This is a patch that adds two functions in order to make it possible to add and remove external
interpreter functions, without using libffi. This is necessary in order to support running calling
external functions from wasm.
