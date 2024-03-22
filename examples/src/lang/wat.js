import { registerLanguage } from './utils'

export function registerWatLanguage(monaco) {
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
