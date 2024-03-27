import { IMonaco, registerLanguage } from './utils'

export function registerRainLanguage(monaco: IMonaco) {
    return registerLanguage(
        monaco,
        'rain',
        {
            tokenizer: {
                root: [
                    // Keywords
                    [/\b(await|break|continue|else|for|if|loop|return|while)\b/, 'keyword'],
                    [/\b(export|fn|import|interface|let|struct|var)\b/, 'keyword'],
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
