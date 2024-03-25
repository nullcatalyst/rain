import { registerRainLanguage } from './lang/rain';
import { registerLlvmIrLanguage } from './lang/llvm-ir';
import { registerWatLanguage } from './lang/wat';

export async function loadEditor() {
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
