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
        {}
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

require.config({
    'paths': {
        'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.47.0/min/vs',
    },
});

require(['vs/editor/editor.main'], async () => {
    registerRainLanguage(monaco);
    registerLlvmIrLanguage(monaco);
    registerWatLanguage(monaco);

    const rainModel = monaco.editor.createModel(document.getElementById('source-code').textContent, 'rain');
    const llvmIrModel = monaco.editor.createModel('; Compile to see output here\n', 'llvm-ir');
    const watModel = monaco.editor.createModel(';; Compile to see output here\n', 'wat');
    let wasmBin = null;

    const editor = monaco.editor.create(document.getElementById('editor'), {
        model: rainModel,
        automaticLayout: true,
        // lineNumbers: 'off',
        // minimap: { enabled: false },
        // padding: { top: 5, right: 5, bottom: 5, left: 5 },
        overviewRulerLanes: 0,
        overviewRulerBorder: false,
        theme: 'vs-dark',
    });

    const rainTab = document.getElementById('rain-tab');
    const llvmIrTab = document.getElementById('llvm-ir-tab');
    const watTab = document.getElementById('wat-tab');

    rainTab.addEventListener('click', () => {
        rainTab.classList.add('active');
        llvmIrTab.classList.remove('active');
        watTab.classList.remove('active');

        editor.setModel(rainModel);
        editor.updateOptions({ readOnly: false });
    });

    llvmIrTab.addEventListener('click', () => {
        rainTab.classList.remove('active');
        llvmIrTab.classList.add('active');
        watTab.classList.remove('active');

        editor.setModel(llvmIrModel);
        editor.updateOptions({ readOnly: true });
    });

    watTab.addEventListener('click', () => {
        rainTab.classList.remove('active');
        llvmIrTab.classList.remove('active');
        watTab.classList.add('active');

        editor.setModel(watModel);
        editor.updateOptions({ readOnly: true });
    });

    const rainc = await load_wasm("rainc.wasm", (event, msg_start, msg_end) => {
        console.log("Received event:", event, "with message:", msg_start, msg_end);

        const data = new Uint8Array(
            rainc.memory.buffer,
            msg_start,
            msg_end - msg_start
        );

        switch (event) {
            case EVENT_COMPILE: {
                const text = new TextDecoder("utf8").decode(data);
                console.log("Compiled LLVM IR:");
                console.log(text);
                llvmIrModel.setValue(text);
                break;
            }

            case EVENT_LINK: {
                console.log("Linked WebAssembly binary:", wasmBin);
                wasmBin = new Uint8Array(data.byteLength);
                wasmBin.set(data);
                break;
            }

            case EVENT_DECOMPILE: {
                const text = new TextDecoder("utf8").decode(data);
                console.log("Compiled WAT:");
                console.log(text);
                watModel.setValue(text);
                break;
            }

            case EVENT_ERROR: {
                const text = new TextDecoder("utf8").decode(data);
                console.error("Error:", text);

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
                    let lastNonSpace = -1;
                    for (const i in underlineSrcMsg) {
                        const c = underlineSrcMsg[i];
                        if (firstNonSpace === -1 && c !== " ") {
                            firstNonSpace = i;
                        } else if (underlineSrcMsg[i - 1] !== " " && c === " ") {
                            lastNonSpace = i;
                        }
                    }

                    monaco.editor.setModelMarkers(
                        monaco.editor.getModels()[0],
                        "compilation",
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
                break;
            }
        }
    });

    rainc.init();

    function compile(src) {
        wasmBin = null;
        console.log(editor);
        monaco.editor.removeAllMarkers("compilation");

        console.log("Compiling source code:");
        console.log(src);

        const encoder = new TextEncoder();
        const encoded = encoder.encode(src);
        const len = encoded.length;
        const ptr = rainc.malloc(len);
        console.log("Allocated memory at:", ptr);
        new Uint8Array(rainc.memory.buffer, ptr, len).set(
            new Uint8Array(encoded)
        );
        rainc.compile(ptr, ptr + len);
        rainc.free(ptr);
    }
    document.getElementById('compile').addEventListener('click', () => {
        const src = monaco.editor.getModels()[0].getValue();
        compile(src);

        if (wasmBin !== null) {
            download.classList.remove("disabled");
        } else {
            download.classList.add("disabled");
        }
    });

    const download = document.getElementById('download');
    download.addEventListener('click', () => {
        if (wasmBin !== null) {
            const blob = new Blob([wasmBin], { type: "application/wasm" });
            const url = URL.createObjectURL(blob);
            const a = document.createElement("a");
            a.href = url;
            a.download = "out.wasm";
            a.click();
        }
    });
});

const EVENT_ERROR = -1;
const EVENT_COMPILE = 1;
const EVENT_LINK = 2;
const EVENT_DECOMPILE = 3;

async function load_wasm(url, callback) {
    let memory;
    const wasm = await WebAssembly.instantiateStreaming(fetch(url), {
        console: {
            log: (msg_start, msg_end) => {
                const text = new TextDecoder("utf8").decode(
                    memory.buffer.slice(msg_start, msg_end)
                );
                console.log(text);
            },
            error: (msg_start, msg_end) => {
                const text = new TextDecoder("utf8").decode(
                    memory.buffer.slice(msg_start, msg_end)
                );
                console.error(text);
            },
        },
        math: Math,
        time: {
            now_perf: () => performance.now(),
            now_unix: () => Date.now(),
        },
        env: {
            callback: (event, msg_start, msg_end) => {
                if (callback) {
                    callback(event, msg_start, msg_end);
                } else {
                    const text = new TextDecoder("utf8").decode(
                        memory.buffer.slice(msg_start, msg_end)
                    );
                    console.log(text);
                }
            },
        },
    });
    memory = wasm.instance.exports.memory;
    return wasm.instance.exports;
}
