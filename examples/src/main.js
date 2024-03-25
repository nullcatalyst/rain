import { loadCompiler } from "./compiler";
import { loadEditor } from "./editor";
import { download } from "./download";

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
