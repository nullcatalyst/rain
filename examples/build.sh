#! /usr/bin/env bash

# Stop on error
set -e

cd $(dirname $0)/..

# Build the tools
bazel build --config wasm -c opt //rain:tools

# Copy the tools to the examples directory
# (-f is used to overwrite the files if they already exist, due to the permissions set on the file
# by bazel)
cp -f bazel-bin/rain/rainc.wasm examples/.
cp -f bazel-bin/rain/rain2llvm.wasm examples/.
cp -f bazel-bin/rain/llvm2wasm.wasm examples/.
cp -f bazel-bin/rain/wasm2wat.wasm examples/.

cd examples

# Pass a port number to start a server to serve the examples (0 will choose a port for you)
if [ ! -z "$1" ]; then
  # Start the server to serve the examples
  if command -v npx &> /dev/null; then
    npx http-server -p "$1"
  else
    echo "npx (npm) is not installed, so could not start the examples server."
    echo "Feel free to use any static file server to test the examples by hosting the examples directory."
  fi
fi

