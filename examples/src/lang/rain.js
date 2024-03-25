import { registerLanguage } from './utils'

export function registerRainLanguage(monaco) {
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
