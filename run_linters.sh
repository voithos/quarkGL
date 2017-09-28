#!/bin/bash

# By default, fail after the first linter that gives warnings.
# If -f is specified, run everything.
if [[ "$1" != "-f" ]]; then
  set -e
fi

cpplint --recursive quarkgl examples
clang-tidy quarkgl/*.cc quarkgl/*.h examples/*.cc examples/*.h
