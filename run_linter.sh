#!/bin/bash
set -e

clang-tidy quarkgl/*.cc quarkgl/*.h examples/*.cc examples/*.h
