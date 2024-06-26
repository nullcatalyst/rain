// EVENT_ERROR will eventually be removed when the compiler outputs a list of diagnostics, instead
// of just a single error message.
const EVENT_ERROR = -1;
const EVENT_UNKNOWN = 0;
const EVENT_COMPILE = 1;
const EVENT_LINK = 2;
const EVENT_DECOMPILE = 3;

const CONSOLE_SUCCESS_STYLE = "color:#32a852;font-weight:bold;";
const CONSOLE_INFO_STYLE = "color:#4287f5;font-weight:bold;";
const CONSOLE_WARNING_STYLE = "color:#fcba03;font-weight:bold;";
const CONSOLE_ERROR_STYLE = "color:#eb4034;font-weight:bold;";

export async function loadCompiler(onCompile, onLink, onDecompile, onError) {
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
                console.log("%cLinked WebAssembly binary:", CONSOLE_INFO_STYLE, out);
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
                console.error("%cError:", CONSOLE_ERROR_STYLE);
                console.error(text);
                onError(text);
                break;
            }
        }
    });

    rainc.init();
    {
        const [ptr, len] = encodeText(rainc.memory, rainc.malloc, "memory");
        rainc.set_memory_export(ptr, ptr + len);
        rainc.free(ptr);
    }
    rainc.set_stack_size(0);

    return function compile(src, optimize) {
        monaco.editor.removeAllMarkers("compilation");

        console.log("%cCompiling source code:", CONSOLE_INFO_STYLE);
        console.log(src);

        const [ptr, len] = encodeText(rainc.memory, rainc.malloc, src);

        // Null-terminate the string.
        // This shouldn't be necessary, but is useful in case of off-by-one bugs in the compiler.
        rainc.compile(ptr, ptr + len, optimize);
        rainc.free(ptr);
    };
}

async function loadWasm(url, callback) {
    let memory;
    const wasm = await WebAssembly.instantiateStreaming(fetch(url), {
        "console": {
            "log": (dataStart, dataEnd) => {
                console.log(decodeText(memory, dataStart, dataEnd));
            },
            "error": (dataStart, dataEnd) => {
                console.error(decodeText(memory, dataStart, dataEnd));
            },
        },
        "math": Math,
        "time": {
            "now_perf": () => performance.now(),
            "now_unix": () => Date.now(),
        },
        "env": {
            "callback": (event, dataStart, dataEnd) => {
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

function encodeText(memory, malloc, text) {
    const encoded = new TextEncoder().encode(text);
    const len = encoded.length;
    const ptr = malloc(len);

    const memoryView = new Uint8Array(memory.buffer, ptr, len);
    memoryView.set(new Uint8Array(encoded));

    return [ptr, len];
}
