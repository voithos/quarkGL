#!/bin/bash
set -e

cpplint --recursive quarkgl examples
clang-tidy quarkgl/*.cc quarkgl/*.h examples/*.cc examples/*.h
