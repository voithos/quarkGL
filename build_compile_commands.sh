#!/bin/bash
set -e

if [[ "$#" -eq 0 ]]; then
  TARGETS[0]="quarkgl:all"
  TARGETS[1]="examples:all"
else
  TARGETS="$@"
fi

bazel build --experimental_action_listener=//tools/actions:generate_compile_commands_listener ${TARGETS[@]}
python3 ./tools/actions/generate_compile_commands_json.py
