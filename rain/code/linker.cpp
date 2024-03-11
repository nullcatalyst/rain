#include "rain/code/linker.hpp"

#include "lld/Common/CommonLinkerContext.h"
#include "lld/Common/Memory.h"
#include "lld/wasm/InputChunks.h"
#include "lld/wasm/InputElement.h"
#include "lld/wasm/InputFiles.h"
#include "lld/wasm/MarkLive.h"
#include "lld/wasm/SymbolTable.h"
#include "rain/code/target.hpp"
#include "rain/err/simple.hpp"
#include "rain/util/defer.hpp"

namespace rain::code {

using namespace lld;
using namespace lld::wasm;
using namespace llvm;
using namespace llvm::wasm;

namespace {

constexpr const size_t DEFAULT_STACK_SIZE = 16 * WasmPageSize;  // 16 pages == 1MiB

void init_config() {
    config->bsymbolic           = false;
    config->checkFeatures       = true;  // List the needed features in the wasm file
    config->compressRelocations = false;
    config->demangle            = true;
    config->disableVerify       = false;
    config->emitRelocs          = false;
    config->experimentalPic     = false;
    // config->entry               = "";
    config->exportAll           = false;
    config->exportTable         = false;
    config->growableTable       = false;
    config->memoryExport        = std::optional<llvm::StringRef>();
    config->sharedMemory        = false;
    config->importTable         = false;
    config->importUndefined     = false;
    config->ltoo                = 2;
    config->ltoCgo              = *CodeGenOpt::getLevel(2);
    config->ltoPartitions       = 1;
    config->ltoDebugPassManager = false;
    config->optimize            = 1;
    config->relocatable         = false;
    config->gcSections          = !config->relocatable;
    config->mergeDataSegments   = !config->relocatable;
    config->pie                 = false;
    config->printGcSections     = false;
    config->saveTemps           = false;
    config->shared              = false;
    config->stripAll            = true;
    config->stripDebug          = true;

    // Put the stack before the static const data, which is before the heap.
    // This is the better option, since as the stack grows (downwards), a stack overflow will result
    // in an access of a negative address, which will be caught by the WASM runtime, rather than
    // resulting in data corruption.
    config->stackFirst = true;

    config->trace      = false;
    config->tableBase  = 0;
    config->globalBase = 0;
    // config->initialHeap       = 0;
    config->initialMemory     = 0;
    config->maxMemory         = 0;
    config->zStackSize        = DEFAULT_STACK_SIZE;
    config->exportDynamic     = config->shared;
    config->is64              = false;
    config->importUndefined   = true;
    config->unresolvedSymbols = UnresolvedPolicy::Ignore;

    config->isPic = config->pie || config->shared;
}

// // Force Sym to be entered in the output. Used for -u or equivalent.
// Symbol* handleUndefined(StringRef name, const char* option) {
//     Symbol* sym = symtab->find(name);
//     if (!sym) return nullptr;

//     // Since symbol S may not be used inside the program, LTO may
//     // eliminate it. Mark the symbol as "used" to prevent it.
//     sym->isUsedInRegularObj = true;

//     if (auto* lazySym = dyn_cast<LazySymbol>(sym)) {
//         lazySym->extract();
//         if (!config->whyExtract.empty())
//             ctx.whyExtractRecords.emplace_back(option, sym->getFile(), *sym);
//     }

//     return sym;
// }

// void handleLibcall(StringRef name) {
//     Symbol* sym = symtab->find(name);
//     if (sym && sym->isLazy() && isa<BitcodeFile>(sym->getFile())) {
//         if (!config->whyExtract.empty())
//             ctx.whyExtractRecords.emplace_back("<libcall>", sym->getFile(), *sym);
//         cast<LazySymbol>(sym)->extract();
//     }
// }

// Equivalent of demote demoteSharedAndLazySymbols() in the ELF linker
void demoteLazySymbols() {
    for (Symbol* sym : symtab->symbols()) {
        if (auto* s = dyn_cast<LazySymbol>(sym)) {
            if (s->signature) {
                // LLVM_DEBUG(llvm::dbgs() << "demoting lazy func: " << s->getName() << "\n");
                replaceSymbol<UndefinedFunction>(s, s->getName(), std::nullopt, std::nullopt,
                                                 WASM_SYMBOL_BINDING_WEAK, s->getFile(),
                                                 s->signature);
            }
        }
    }
}

UndefinedGlobal* createUndefinedGlobal(StringRef name, llvm::wasm::WasmGlobalType* type) {
    auto* sym = cast<UndefinedGlobal>(symtab->addUndefinedGlobal(
        name, std::nullopt, std::nullopt, WASM_SYMBOL_UNDEFINED, nullptr, type));
    config->allowUndefinedSymbols.insert(sym->getName());
    sym->isUsedInRegularObj = true;
    return sym;
}

InputGlobal* createGlobal(StringRef name, bool isMutable) {
    llvm::wasm::WasmGlobal wasmGlobal;
    bool                   is64 = config->is64.value_or(false);
    wasmGlobal.Type             = {uint8_t(is64 ? WASM_TYPE_I64 : WASM_TYPE_I32), isMutable};
    wasmGlobal.InitExpr         = intConst(0, is64);
    wasmGlobal.SymbolName       = name;
    return make<InputGlobal>(wasmGlobal, nullptr);
}

GlobalSymbol* createGlobalVariable(StringRef name, bool isMutable) {
    InputGlobal* g = createGlobal(name, isMutable);
    return symtab->addSyntheticGlobal(name, WASM_SYMBOL_VISIBILITY_HIDDEN, g);
}

// GlobalSymbol* createOptionalGlobal(StringRef name, bool isMutable) {
//     InputGlobal* g = createGlobal(name, isMutable);
//     return symtab->addOptionalGlobalSymbol(name, g);
// }

// Create ABI-defined synthetic symbols
void createSyntheticSymbols() {
    if (config->relocatable) return;

    static WasmSignature  nullSignature        = {{}, {}};
    static WasmSignature  i32ArgSignature      = {{}, {ValType::I32}};
    static WasmSignature  i64ArgSignature      = {{}, {ValType::I64}};
    static WasmGlobalType globalTypeI32        = {WASM_TYPE_I32, false};
    static WasmGlobalType globalTypeI64        = {WASM_TYPE_I64, false};
    static WasmGlobalType mutableGlobalTypeI32 = {WASM_TYPE_I32, true};
    static WasmGlobalType mutableGlobalTypeI64 = {WASM_TYPE_I64, true};
    WasmSym::callCtors =
        symtab->addSyntheticFunction("__wasm_call_ctors", WASM_SYMBOL_VISIBILITY_HIDDEN,
                                     make<SyntheticFunction>(nullSignature, "__wasm_call_ctors"));

    bool is64 = config->is64.value_or(false);

    if (config->isPic) {
        WasmSym::stackPointer = createUndefinedGlobal(
            "__stack_pointer",
            config->is64.value_or(false) ? &mutableGlobalTypeI64 : &mutableGlobalTypeI32);
        // For PIC code, we import two global variables (__memory_base and
        // __table_base) from the environment and use these as the offset at
        // which to load our static data and function table.
        // See:
        // https://github.com/WebAssembly/tool-conventions/blob/main/DynamicLinking.md
        auto* globalType    = is64 ? &globalTypeI64 : &globalTypeI32;
        WasmSym::memoryBase = createUndefinedGlobal("__memory_base", globalType);
        WasmSym::tableBase  = createUndefinedGlobal("__table_base", globalType);
        WasmSym::memoryBase->markLive();
        WasmSym::tableBase->markLive();
        if (is64) {
            WasmSym::tableBase32 = createUndefinedGlobal("__table_base32", &globalTypeI32);
            WasmSym::tableBase32->markLive();
        } else {
            WasmSym::tableBase32 = nullptr;
        }
    } else {
        // For non-PIC code
        WasmSym::stackPointer = createGlobalVariable("__stack_pointer", true);
        WasmSym::stackPointer->markLive();
    }

    if (config->sharedMemory) {
        WasmSym::tlsBase  = createGlobalVariable("__tls_base", true);
        WasmSym::tlsSize  = createGlobalVariable("__tls_size", false);
        WasmSym::tlsAlign = createGlobalVariable("__tls_align", false);
        WasmSym::initTLS  = symtab->addSyntheticFunction(
            "__wasm_init_tls", WASM_SYMBOL_VISIBILITY_HIDDEN,
            make<SyntheticFunction>(is64 ? i64ArgSignature : i32ArgSignature, "__wasm_init_tls"));
    }

    if (config->isPic || config->unresolvedSymbols == UnresolvedPolicy::ImportDynamic) {
        // For PIC code, or when dynamically importing addresses, we create
        // synthetic functions that apply relocations.  These get called from
        // __wasm_call_ctors before the user-level constructors.
        WasmSym::applyDataRelocs = symtab->addSyntheticFunction(
            "__wasm_apply_data_relocs", WASM_SYMBOL_VISIBILITY_DEFAULT | WASM_SYMBOL_EXPORTED,
            make<SyntheticFunction>(nullSignature, "__wasm_apply_data_relocs"));
    }
}

void createOptionalSymbols() {
    if (config->relocatable) return;

    // WasmSym::dsoHandle = symtab->addOptionalDataSymbol("__dso_handle");

    // if (!config->shared) {
    //     WasmSym::dataEnd = symtab->addOptionalDataSymbol("__data_end");
    // }

    // if (!config->isPic) {
    //     WasmSym::stackLow          = symtab->addOptionalDataSymbol("__stack_low");
    //     WasmSym::stackHigh         = symtab->addOptionalDataSymbol("__stack_high");
    //     WasmSym::globalBase        = symtab->addOptionalDataSymbol("__global_base");
    //     WasmSym::heapBase          = symtab->addOptionalDataSymbol("__heap_base");
    //     WasmSym::heapEnd           = symtab->addOptionalDataSymbol("__heap_end");
    //     WasmSym::definedMemoryBase = symtab->addOptionalDataSymbol("__memory_base");
    //     WasmSym::definedTableBase  = symtab->addOptionalDataSymbol("__table_base");
    //     if (config->is64.value_or(false))
    //         WasmSym::definedTableBase32 = symtab->addOptionalDataSymbol("__table_base32");
    // }

    // // For non-shared memory programs we still need to define __tls_base since we
    // // allow object files built with TLS to be linked into single threaded
    // // programs, and such object files can contain references to this symbol.
    // //
    // // However, in this case __tls_base is immutable and points directly to the
    // // start of the `.tdata` static segment.
    // //
    // // __tls_size and __tls_align are not needed in this case since they are only
    // // needed for __wasm_init_tls (which we do not create in this case).
    // if (!config->sharedMemory) {
    //     WasmSym::tlsBase = createOptionalGlobal("__tls_base", false);
    // }
}

void processStubLibrariesPreLTO() {
    log("-- processStubLibrariesPreLTO");
    for (auto& stub_file : symtab->stubFiles) {
        // LLVM_DEBUG(llvm::dbgs() << "processing stub file: " << stub_file->getName() << "\n");
        for (auto [name, deps] : stub_file->symbolDependencies) {
            auto* sym = symtab->find(name);
            // If the symbol is not present at all (yet), or if it is present but
            // undefined, then mark the dependent symbols as used by a regular
            // object so they will be preserved and exported by the LTO process.
            if (!sym || sym->isUndefined()) {
                for (const auto dep : deps) {
                    auto* needed = symtab->find(dep);
                    if (needed) {
                        needed->isUsedInRegularObj = true;
                    }
                }
            }
        }
    }
}

void processStubLibraries() {
    log("-- processStubLibraries");
    for (auto& stub_file : symtab->stubFiles) {
        // LLVM_DEBUG(llvm::dbgs() << "processing stub file: " << stub_file->getName() << "\n");
        for (auto [name, deps] : stub_file->symbolDependencies) {
            auto* sym = symtab->find(name);
            if (!sym || !sym->isUndefined()) {
                // LLVM_DEBUG(llvm::dbgs() << "stub symbol not needed: " << name << "\n");
                continue;
            }
            // The first stub library to define a given symbol sets this and
            // definitions in later stub libraries are ignored.
            if (sym->forceImport) continue;  // Already handled
            sym->forceImport = true;
            if (sym->traced) message(toString(stub_file) + ": importing " + name);
            // else
            //     LLVM_DEBUG(llvm::dbgs() << toString(stub_file) << ": importing " << name <<
            //     "\n");
            for (const auto dep : deps) {
                auto* needed = symtab->find(dep);
                if (!needed) {
                    error(toString(stub_file) + ": undefined symbol: " + dep + ". Required by " +
                          toString(*sym));
                } else if (needed->isUndefined()) {
                    error(toString(stub_file) + ": undefined symbol: " + toString(*needed) +
                          ". Required by " + toString(*sym));
                } else {
                    if (needed->traced)
                        message(toString(stub_file) + ": exported " + toString(*needed) +
                                " due to import of " + name);
                    // else
                    //     LLVM_DEBUG(llvm::dbgs() << "force export: " << toString(*needed) <<
                    //     "\n");
                    needed->forceExport = true;
                    if (auto* lazy = dyn_cast<LazySymbol>(needed)) {
                        lazy->fetch();
                        if (!config->whyExtract.empty())
                            config->whyExtractRecords.emplace_back(stub_file->getName(),
                                                                   sym->getFile(), *sym);
                    }
                }
            }
        }
    }
    log("-- done processStubLibraries");
}

// The --wrap option is a feature to rename symbols so that you can write
// wrappers for existing functions. If you pass `-wrap=foo`, all
// occurrences of symbol `foo` are resolved to `wrap_foo` (so, you are
// expected to write `wrap_foo` function as a wrapper). The original
// symbol becomes accessible as `real_foo`, so you can call that from your
// wrapper.
//
// This data structure is instantiated for each -wrap option.
struct WrappedSymbol {
    Symbol* sym;
    Symbol* real;
    Symbol* wrap;
};

// Symbol* addUndefined(StringRef name) {
//     return symtab->addUndefinedFunction(name, std::nullopt, std::nullopt, WASM_SYMBOL_UNDEFINED,
//                                         nullptr, nullptr, false);
// }

// // Do renaming for -wrap by updating pointers to symbols.
// //
// // When this function is executed, only InputFiles and symbol table
// // contain pointers to symbol objects. We visit them to replace pointers,
// // so that wrapped symbols are swapped as instructed by the command line.
// void wrapSymbols(ArrayRef<WrappedSymbol> wrapped) {
//     DenseMap<Symbol*, Symbol*> map;
//     for (const WrappedSymbol& w : wrapped) {
//         map[w.sym]  = w.wrap;
//         map[w.real] = w.sym;
//     }

//     // Update pointers in input files.
//     // parallelForEach(ctx.objectFiles, [&](InputFile* file) {
//     for (InputFile* file : ctx.objectFiles) {
//         MutableArrayRef<Symbol*> syms = file->getMutableSymbols();
//         for (size_t i = 0, e = syms.size(); i != e; ++i)
//             if (Symbol* s = map.lookup(syms[i])) syms[i] = s;
//     }
//     // });

//     // Update pointers in the symbol table.
//     for (const WrappedSymbol& w : wrapped) symtab->wrap(w.sym, w.real, w.wrap);
// }

void splitSections() {
    // splitIntoPieces needs to be called on each MergeInputChunk before calling finalizeContents().
    // LLVM_DEBUG(llvm::dbgs() << "splitSections\n");
    // parallelForEach(ctx.objectFiles, [](ObjFile* file) {
    for (ObjFile* file : symtab->objectFiles) {
        for (InputChunk* seg : file->segments) {
            if (auto* s = dyn_cast<MergeInputChunk>(seg)) s->splitIntoPieces();
        }
        for (InputChunk* sec : file->customSections) {
            if (auto* s = dyn_cast<MergeInputChunk>(sec)) s->splitIntoPieces();
        }
    }
    // });
}

}  // namespace

std::unique_ptr<llvm::MemoryBuffer> writeResult();

util::Result<std::unique_ptr<llvm::MemoryBuffer>> Linker::link() {
    lld::CommonLinkerContext _ctx;
    auto                     _config = std::make_unique<lld::wasm::Configuration>();
    auto                     _symtab = std::make_unique<lld::wasm::SymbolTable>();
    assert(_symtab->symbols().empty() && "SzymbolTable should be empty");
    assert(_symtab->find("__stack_pointer") == nullptr && "stack_pointer should not exist");
    lld::wasm::config = _config.get();
    lld::wasm::symtab = _symtab.get();
    rain::util::Defer reset_lld_globals([] {
        lld::wasm::config = nullptr;
        lld::wasm::symtab = nullptr;
    });
    init_config();
    // ctx.reset();

    createSyntheticSymbols();

    {
        lld::wasm::config->zStackSize = _stack_size != 0 ? _stack_size : DEFAULT_STACK_SIZE;

        for (auto& file : _files) {
            lld::wasm::symtab->addFile(lld::wasm::createObjectFile(*file));
        }

        for (auto& function_name : _force_export_symbols) {
            lld::wasm::config->exportedSymbols.insert(function_name);
        }
    }

    // We process the stub libraries once beofore LTO to ensure that any possible
    // required exports are preserved by the LTO process.
    processStubLibrariesPreLTO();

    symtab->compileBitcodeFiles();

    // The LTO process can generate new undefined symbols, specifically libcall
    // functions.  Because those symbols might be declared in a stub library we
    // need the process the stub libraries once again after LTO to handle all
    // undefined symbols, including ones that didn't exist prior to LTO.
    processStubLibraries();

    // writeWhyExtract();

    createOptionalSymbols();

    // Resolve any variant symbols that were created due to signature mismatchs.
    symtab->handleSymbolVariants();
    if (errorCount()) {
        return ERR_PTR(err::SimpleError, "errors encountered during linking");
    }

    // Split WASM_SEG_FLAG_STRINGS sections into pieces in preparation for garbage collection.
    splitSections();

    // Any remaining lazy symbols should be demoted to Undefined.
    demoteLazySymbols();

    // Do size optimizations: garbage collection.
    markLive();

    // Provide the indirect function table if needed.
    WasmSym::indirectFunctionTable = symtab->resolveIndirectFunctionTable(/*required =*/false);

    return writeResult();
}

util::Result<void> Linker::add(llvm::Module& llvm_mod) {
    llvm::SmallString<0>      code;
    llvm::raw_svector_ostream ostream(code);
    llvm::legacy::PassManager pass_manager;

    auto target_machine = wasm_target_machine();
    if (target_machine->addPassesToEmitFile(pass_manager, ostream, nullptr,
                                            llvm::CodeGenFileType::CGFT_ObjectFile)) {
        return ERR_PTR(err::SimpleError, "failed to emit object file");
    }
    pass_manager.run(llvm_mod);

    // Based on the documentation for llvm::raw_svector_ostream, the underlying SmallString is
    // always up to date, so there is no need to call flush().
    // ostream.flush();
    add(llvm::MemoryBuffer::getMemBufferCopy(code.str()));
    return {};
}

}  // namespace rain::code
