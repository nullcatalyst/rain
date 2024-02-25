#pragma once

#if defined(__wasm__)

#define WASM_IMPORT($module, $name) __attribute__((import_module($module), import_name($name)))
#define WASM_EXPORT($name) __attribute__((export_name($name)))

#else  // defined(__wasm__)

#define WASM_IMPORT($module, $name)
#define WASM_EXPORT($name)

#endif  // !defined(__wasm__)
