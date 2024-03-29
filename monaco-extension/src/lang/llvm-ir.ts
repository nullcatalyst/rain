import { IMonaco, registerLanguage } from './utils'

export function registerLlvmIrLanguage(monaco: typeof import('monaco-editor')) {
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
