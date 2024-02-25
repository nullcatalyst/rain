#! /usr/bin/env bash

# Stop on error
set -e

cd $(dirname $0)/..

# Build the tools
bazel build --config wasm -c opt //rain:rain2llvm //rain:llvm2wasm //rain:wasm2wat

# Copy the tools to the examples directory
# (-f is used to overwrite the files if they already exist, due to the permissions set on the file
# by bazel)
cp -f bazel-bin/rain/rain2llvm.wasm examples/.
cp -f bazel-bin/rain/llvm2wasm.wasm examples/.
cp -f bazel-bin/rain/wasm2wat.wasm examples/.

cd examples

# Start the server to serve the examples
if command -v npx &> /dev/null; then
  npx http-server
else
  echo "npx (npm) is not installed, so could not start the examples server."
  echo "Feel free to use any static file server to test the examples by hosting the examples directory."
fi
