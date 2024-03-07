import * as monaco from 'monaco-editor';
import { registerRainLanguage } from './lang/rain';
import { registerLlvmIrLanguage } from './lang/llvm-ir';
import { registerWatLanguage } from './lang/wat';

registerLlvmIrLanguage(monaco);
registerWatLanguage(monaco);
registerRainLanguage(monaco);