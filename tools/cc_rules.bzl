"""
This file is used to define wrappers for cc_library and cc_binary rules.

These wrappers are used to generate build rules for C/C++ targets, that only pass the srcs to be
compiled when targetting WebAssembly.

This is useful because it creates empty builds for non-WebAssembly targets, so that they can still
be included in the list of deps without affecting non-WebAssembly builds.
"""

def cc_library(name, deps = [], **kwargs):
    native.cc_library(
        name = name,
        # Implicitly depend on the WebAssembly C/C++ standard libraries
        deps = deps + [
            "@wasm_toolchain//c",
            "@wasm_toolchain//c++",
        ],
        **kwargs
    )

def cc_binary(name, deps = [], **kwargs):
    native.cc_binary(
        name = name,
        # Implicitly depend on the WebAssembly C/C++ standard libraries
        deps = deps + [
            "@wasm_toolchain//c",
            "@wasm_toolchain//c++",
        ],
        **kwargs
    )
