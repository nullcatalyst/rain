// EVENT_ERROR will eventually be removed when the compiler outputs a list of diagnostics, instead
// of just a single error message.
const EVENT_ERROR = -1;
const EVENT_COMPILE = 1;
const EVENT_LINK = 2;
const EVENT_DECOMPILE = 3;

const CONSOLE_SUCCESS_STYLE = "color: #32a852; font-weight: bold;";
const CONSOLE_INFO_STYLE = "color: #4287f5; font-weight: bold;";
const CONSOLE_ERROR_STYLE = "color: #eb4034; font-weight: bold;";

async function loadCompiler(onCompile, onLink, onDecompile, onError) {
    const rainc = await loadWasm("rainc.wasm", (event, dataStart, dataEnd) => {
        switch (event) {
            case EVENT_COMPILE: {
                const text = decodeText(rainc.memory, dataStart, dataEnd);
                console.log("%cCompiled LLVM IR:", CONSOLE_INFO_STYLE);
                console.log(text);
                onCompile(text);
                break;
            }

            case EVENT_LINK: {
                // Create a copy of the data, since the passed in data will be deallocated after.
                const data = memoryView(rainc.memory, dataStart, dataEnd);
                const out = new Uint8Array(data.byteLength);
                out.set(data);
                console.log("%Linked WebAssembly binary:", CONSOLE_INFO_STYLE, out);
                onLink(out);
                break;
            }

            case EVENT_DECOMPILE: {
                const text = decodeText(rainc.memory, dataStart, dataEnd);
                console.log("%cCompiled WAT:", CONSOLE_INFO_STYLE);
                console.log(text);
                onDecompile(text);
                console.log("%cSuccess!", CONSOLE_SUCCESS_STYLE);
                break;
            }

            case EVENT_ERROR: {
                const text = decodeText(rainc.memory, dataStart, dataEnd);
                console.error("%cError:", CONSOLE_ERROR_STYLE, text);
                onError(text);
                break;
            }
        }
    });

    rainc.init();

    return function compile(src, optimize) {
        monaco.editor.removeAllMarkers("compilation");

        console.log("%cCompiling source code:", CONSOLE_INFO_STYLE);
        console.log(src);

        const encoded = new TextEncoder().encode(src);
        const len = encoded.length;
        const ptr = rainc.malloc(len + 1);

        const memoryView = new Uint8Array(rainc.memory.buffer, ptr, len);
        memoryView.set(new Uint8Array(encoded));
        // Null-terminate the string.
        // This shouldn't be necessary, but is useful in case of off-by-one bugs in the compiler.
        memoryView[len] = 0;
        rainc.compile(ptr, ptr + len, optimize);
        rainc.free(ptr);
    };
}

async function loadWasm(url, callback) {
    let memory;
    const wasm = await WebAssembly.instantiateStreaming(fetch(url), {
        console: {
            log: (dataStart, dataEnd) => {
                console.log(decodeText(memory, dataStart, dataEnd));
            },
            error: (dataStart, dataEnd) => {
                console.error(decodeText(memory, dataStart, dataEnd));
            },
        },
        math: Math,
        time: {
            now_perf: () => performance.now(),
            now_unix: () => Date.now(),
        },
        env: {
            callback: (event, dataStart, dataEnd) => {
                if (callback) {
                    callback(event, dataStart, dataEnd);
                } else {
                    console.log(decodeText(memory, dataStart, dataEnd));
                }
            },
        },
    });
    memory = wasm.instance.exports.memory;
    return wasm.instance.exports;
}

function memoryView(memory, start, end) {
    return new Uint8Array(memory.buffer, start, end - start);
}

function decodeText(memory, start, end) {
    return new TextDecoder("utf8").decode(memoryView(memory, start, end));
}

function registerLanguage(monaco, langId, tokensProvider, config) {
    // if (languageIsInitialized(monaco, langId)) {
    //     return;
    // }

    monaco.languages.register({
        id: langId,
    });
    monaco.languages.setMonarchTokensProvider(langId, tokensProvider);
    monaco.languages.setLanguageConfiguration(langId, config);
}

function registerRainLanguage(monaco) {
    return registerLanguage(
        monaco,
        'rain',
        {
            tokenizer: {
                root: [
                    // Keywords
                    [/\b(await|break|continue|else|for|if|loop|return|while)\b/, 'keyword'],
                    [/\b(export|fn|import|interface|let|struct|var|self)\b/, 'keyword'],
                    // Comments
                    [/\/\/.*$/, 'comment'],
                    // Strings
                    [/"(?:[^"\\]|\\.)*"/, 'string'],
                    // Numbers
                    [/\d+/, 'number'],
                    [/\d+.\d+/, 'number'],
                    // Identifiers
                    [/[A-Za-z_][0-9A-Za-z_]*/, 'identifier'],
                    // Whitespace
                    [/\s+/, 'whitespace'],
                ],
            },
        },
        {
            comments: {
                lineComment: "//",
                // blockComment: [ "/*", "*/" ]
            },
        }
    )
}

function registerLlvmIrLanguage(monaco) {
    return registerLanguage(
        monaco,
        'llvm-ir',
        {
            types: [
                'void',
                'half',
                'float',
                'double',
                'x86_fp80',
                'fp128',
                'ppc_fp128',
                'label',
                'metadata',
                'x86_mmx',
                'type',
                'label',
                'opaque',
                'token',
            ],
            statements: [
                'add',
                'addrspacecast',
                'alloca',
                'and',
                'arcp',
                'ashr',
                'atomicrmw',
                'bitcast',
                'br',
                'catchpad',
                'catchswitch',
                'catchret',
                'call',
                'cleanuppad',
                'cleanupret',
                'cmpxchg',
                'eq',
                'exact',
                'extractelement',
                'extractvalue',
                'fadd',
                'fast',
                'fcmp',
                'fdiv',
                'fence',
                'fmul',
                'fpext',
                'fptosi',
                'fptoui',
                'fptrunc',
                'free',
                'frem',
                'fsub',
                'getelementptr',
                'icmp',
                'inbounds',
                'indirectbr',
                'insertelement',
                'insertvalue',
                'inttoptr',
                'invoke',
                'landingpad',
                'load',
                'lshr',
                'malloc',
                'max',
                'min',
                'mul',
                'nand',
                'ne',
                'ninf',
                'nnan',
                'nsw',
                'nsz',
                'nuw',
                'oeq',
                'oge',
                'ogt',
                'ole',
                'olt',
                'one',
                'or',
                'ord',
                'phi',
                'ptrtoint',
                'resume',
                'ret',
                'sdiv',
                'select',
                'sext',
                'sge',
                'sgt',
                'shl',
                'shufflevector',
                'sitofp',
                'sle',
                'slt',
                'srem',
                'store',
                'sub',
                'switch',
                'trunc',
                'udiv',
                'ueq',
                'uge',
                'ugt',
                'uitofp',
                'ule',
                'ult',
                'umax',
                'umin',
                'une',
                'uno',
                'unreachable',
                'unwind',
                'urem',
                'va_arg',
                'xchg',
                'xor',
                'zext',
            ],
            keywords: [
                'acq_rel',
                'acquire',
                'addrspace',
                'alias',
                'align',
                'alignstack',
                'alwaysinline',
                'appending',
                'argmemonly',
                'arm_aapcscc',
                'arm_aapcs_vfpcc',
                'arm_apcscc',
                'asm',
                'atomic',
                'available_externally',
                'blockaddress',
                'builtin',
                'byval',
                'c',
                'catch',
                'caller',
                'cc',
                'ccc',
                'cleanup',
                'coldcc',
                'comdat',
                'common',
                'constant',
                'datalayout',
                'declare',
                'default',
                'define',
                'deplibs',
                'dereferenceable',
                'distinct',
                'dllexport',
                'dllimport',
                'dso_local',
                'dso_preemptable',
                'except',
                'external',
                'externally_initialized',
                'extern_weak',
                'fastcc',
                'filter',
                'from',
                'gc',
                'global',
                'hhvmcc',
                'hhvm_ccc',
                'hidden',
                'initialexec',
                'inlinehint',
                'inreg',
                'inteldialect',
                'intel_ocl_bicc',
                'internal',
                'linkonce',
                'linkonce_odr',
                'localdynamic',
                'localexec',
                'local_unnamed_addr',
                'minsize',
                'module',
                'monotonic',
                'msp430_intrcc',
                'musttail',
                'naked',
                'nest',
                'noalias',
                'nobuiltin',
                'nocapture',
                'noimplicitfloat',
                'noinline',
                'nonlazybind',
                'nonnull',
                'norecurse',
                'noredzone',
                'noreturn',
                'nounwind',
                'optnone',
                'optsize',
                'personality',
                'private',
                'protected',
                'ptx_device',
                'ptx_kernel',
                'readnone',
                'readonly',
                'release',
                'returned',
                'returns_twice',
                'sanitize_address',
                'sanitize_memory',
                'sanitize_thread',
                'section',
                'seq_cst',
                'sideeffect',
                'signext',
                'syncscope',
                'source_filename',
                'speculatable',
                'spir_func',
                'spir_kernel',
                'sret',
                'ssp',
                'sspreq',
                'sspstrong',
                'strictfp',
                'swiftcc',
                'tail',
                'target',
                'thread_local',
                'to',
                'triple',
                'unnamed_addr',
                'unordered',
                'uselistorder',
                'uselistorder_bb',
                'uwtable',
                'volatile',
                'weak',
                'weak_odr',
                'within',
                'writeonly',
                'x86_64_sysvcc',
                'win64cc',
                'x86_fastcallcc',
                'x86_stdcallcc',
                'x86_thiscallcc',
                'zeroext',
            ],
            escapes: /\\(?:[abfnrtv\\"']|x[0-9A-Fa-f]{1,4}|u[0-9A-Fa-f]{4}|U[0-9A-Fa-f]{8})/,
            tokenizer: {
                root: [
                    [/[,(){}<>[\]]/, 'delimiters'],
                    [/i\d+\**/, 'type'],
                    [/[%@!]\d+/, 'variable.name'],
                    [/-?\d+\.\d*(e[+-]\d+)?/, 'number.float'],
                    [/0[xX][0-9A-Fa-f]+/, 'number.hex'],
                    [/-?\d+/, 'number'],
                    [/\b(true|false)\b/, 'keyword'],
                    [/\b(zeroinitializer|undef|null|none)\b/, 'constant'],
                    [/"([^"\\]|\\.)*$/, 'string.invalid'],
                    [/"/, 'string', '@string'],
                    [/[-a-zA-Z$._][-a-zA-Z$._0-9]*(?:\([^:]+\))?:/, 'tag'],
                    [/[%@][-a-zA-Z$._][-a-zA-Z$._0-9]*/, 'variable.name'],
                    [/![-a-zA-Z$._][-a-zA-Z$._0-9]*(?=\s*)$/, 'identifier'],
                    [/![-a-zA-Z$._][-a-zA-Z$._0-9]*(?=\s*[=!])/, 'identifier'],
                    [/![A-Za-z]+(?=\s*\()/, 'type'],
                    [/\bDW_TAG_[a-z_]+\b/, 'constant'],
                    [/\bDW_ATE_[a-zA-Z_]+\b/, 'constant'],
                    [/\bDW_OP_[a-zA-Z0-9_]+\b/, 'constant'],
                    [/\bDW_LANG_[a-zA-Z0-9_]+\b/, 'constant'],
                    [/\bDW_VIRTUALITY_[a-z_]+\b/, 'constant'],
                    [/\bDIFlag[A-Za-z]+\b/, 'constant'],
                    [/;\s*PR\d*\s*$/, 'comment.doc'],
                    [/;\s*REQUIRES:.*$/, 'comment.doc'],
                    [/;\s*RUN:.*$/, 'comment.doc'],
                    [/;\s*CHECK:.*$/, 'comment.doc'],
                    [/;\s*CHECK-(?:NEXT|NOT|DAG|SAME|LABEL):.*$/, 'comment.doc'],
                    [/;\s*XFAIL:.*$/, 'comment.doc'],
                    [/;.*$/, 'comment'],
                    [/[*#=!]/, 'operators'],
                    [
                        /[a-z_$][\w$]*/,
                        {
                            cases: {
                                '@statements': 'operators',
                                '@keywords': 'keyword',
                                '@types': 'type',
                                '@default': 'identifier',
                            },
                        },
                    ],
                    [/[ \t\r\n]+/, 'white'],
                ],
                string: [
                    [/[^\\"]+/, 'string'],
                    [/@escapes/, 'string.escape'],
                    [/\\./, 'string.escape.invalid'],
                    [/"/, 'string', '@pop'],
                ],
            },
        },
        {
            comments: {
                lineComment: ';',
            },
            brackets: [
                ['{', '}'],
                ['[', ']'],
                ['(', ')'],
            ],
            autoClosingPairs: [
                { open: '[', close: ']' },
                { open: '{', close: '}' },
                { open: '(', close: ')' },
                { open: "'", close: "'", notIn: ['string', 'comment'] },
                { open: '"', close: '"', notIn: ['string'] },
            ],
            surroundingPairs: [
                { open: '{', close: '}' },
                { open: '[', close: ']' },
                { open: '(', close: ')' },
                { open: '"', close: '"' },
                { open: "'", close: "'" },
            ],
        }
    )
}

function registerWatLanguage(monaco) {
    return registerLanguage(
        monaco,
        'wat',
        {
            tokenizer: {
                root: [
                    // Comments
                    [/\;\;.*$/, 'comment'],
                    // S-Expression opening
                    [/\(/, 'delimiter.paren.open'],
                    // S-Expression closing
                    [/\)/, 'delimiter.paren.close'],
                    // Keywords
                    [/\b(module|export|func|memory|global|data|mut|imm)\b/, 'keyword'],
                    // Numbers (i32, i64, etc.)
                    [/\b(i32|i64|f32|f64)\b/, 'number.type'],
                    // Strings and identifiers
                    [/"(?:[^"\\]|\\.)*"/, 'string'],
                    [/[a-zA-Z_][\w-]*/, 'identifier'],
                    // Whitespace
                    [/\s+/, 'white'],
                ],
            },
        },
        {}
    )
}

async function loadEditor() {
    require.config({
        'paths': {
            'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.47.0/min/vs',
        },
    });

    return new Promise((resolve, reject) => {
        require(['vs/editor/editor.main'], async () => {
            registerRainLanguage(monaco);
            registerLlvmIrLanguage(monaco);
            registerWatLanguage(monaco);

            const sourceCode = document.getElementById('source-code').textContent;
            const rain = monaco.editor.createModel(sourceCode, 'rain');
            const llvmIr = monaco.editor.createModel('; Compile to see output here\n', 'llvm-ir');
            const wat = monaco.editor.createModel(';; Compile to see output here\n', 'wat');

            const editor = monaco.editor.create(document.getElementById('editor'), {
                model: rain,
                automaticLayout: true,
                overviewRulerLanes: 0,
                overviewRulerBorder: false,
                theme: 'vs-dark',
            });

            resolve({
                monaco,
                editor,
                rain,
                llvmIr,
                wat,
            });
        });
    });
}

function download(fileName, data, mimeType) {
    const blob = new Blob([data], { type: mimeType });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = fileName;
    a.click();
}

async function main() {
    let wasm = null;
    const onCompile = (text) => { llvmIr.setValue(text); };
    const onLink = (_wasm) => { wasm = _wasm; };
    const onDecompile = (text) => { wat.setValue(text); };
    const onError = (text) => {
        debugger;
        const parseErrorMsg = (msg) => {
            const parts = msg.split(":");
            const fileName = parts[0];
            const line = Number(parts[1]);
            const column = Number(parts[2]);
            const message = parts.slice(3).join(":");
            return [fileName, line, column, message];
        };

        if (text.startsWith("<unknown>:")) {
            const lines = text.split("\n");
            const [_fileName, line, column, message] = parseErrorMsg(lines[0]);

            const underlineSrcMsg = lines[2];
            let firstNonSpace = -1;
            let lastNonSpace = underlineSrcMsg.length;
            for (let i = 0; i < underlineSrcMsg.length; ++i) {
                const c = underlineSrcMsg[i];
                if (firstNonSpace === -1 && c !== " ") {
                    firstNonSpace = i;
                } else if (underlineSrcMsg[i - 1] !== " " && c === " ") {
                    lastNonSpace = i;
                }
            }

            monaco.editor.setModelMarkers(
                wat,
                "error",
                [
                    {
                        startLineNumber: line,
                        startColumn: column,
                        endLineNumber: line,
                        endColumn: lastNonSpace,
                        message: message,
                        severity: monaco.MarkerSeverity.Error,
                    },
                ]
            );
        }
    };

    const [_editor, compile] = await Promise.all([
        loadEditor(),
        loadCompiler(onCompile, onLink, onDecompile, onError),
    ]);
    const { monaco, editor, rain, llvmIr, wat } = _editor;
    const models = [rain, llvmIr, wat];

    const rainTab = document.getElementById('rain-tab');
    const llvmIrTab = document.getElementById('llvm-ir-tab');
    const watTab = document.getElementById('wat-tab');
    const tabs = [rainTab, llvmIrTab, watTab];

    const compileButton = document.getElementById('compile');
    const downloadButton = document.getElementById('download');
    const optimizeCheckbox = document.getElementById('optimize');

    // Change to the correct tab on click.
    tabs.forEach((tab, i) => {
        tab.addEventListener('click', () => {
            tabs.forEach((tab) => tab.classList.remove('active'));
            tab.classList.add('active');

            editor.setModel(models[i]);
            editor.updateOptions({ readOnly: i !== 0 });
        });
    });

    compileButton.addEventListener('click', () => {
        compile(rain.getValue(), optimizeCheckbox.checked);

        if (wasm !== null) {
            downloadButton.classList.remove("disabled");
        } else {
            downloadButton.classList.add("disabled");
        }
    });

    downloadButton.addEventListener('click', () => {
        if (wasm !== null) {
            download("out.wasm", wasm, "application/wasm");
        }
    });
}

main();
