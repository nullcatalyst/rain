import { registerRainLanguage } from './lang/rain';
import { registerLlvmIrLanguage } from './lang/llvm-ir';
import { registerWatLanguage } from './lang/wat';

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
    const compileButton = document.getElementById('compile');
    const optimizeCheckbox = document.getElementById('optimize');

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
                wasmBin = new Uint8Array(data.byteLength);
                wasmBin.set(data);
                console.log("Linked WebAssembly binary:", wasmBin);
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
                } else {

                }
                break;
            }
        }
    });

    rainc.init();

    function compile(src) {
        wasmBin = null;
        monaco.editor.removeAllMarkers("compilation");

        console.log("Compiling source code:");
        console.log(src);

        const encoder = new TextEncoder();
        const encoded = encoder.encode(src);
        const len = encoded.length;
        const ptr = rainc.malloc(len + 1);
        console.log("Allocated memory at:", ptr);

        const memoryView = new Uint8Array(rainc.memory.buffer, ptr, len);
        memoryView.set(new Uint8Array(encoded));
        memoryView[len] = 0; // Null-terminate the string.
        rainc.compile(ptr, ptr + len, optimizeCheckbox.checked);
        rainc.free(ptr);
    };

    compileButton.addEventListener('click', () => {
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

