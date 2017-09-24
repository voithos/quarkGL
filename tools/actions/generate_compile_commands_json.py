#!/usr/bin/python3

# This reads the _compile_command files :generate_compile_commands_action
# outputs a compile_commands.json file at the top of the source tree for things
# like clang-tidy to read.

# Overall usage directions: run Bazel with
# --experimental_action_listener=//tools/actions:generate_compile_commands_listener
# for all the files you want to use clang-tidy with and then run this script.
# After that, `clang-tidy build_tests/gflags.cc` should work.

import sys
import pathlib
import os.path
import subprocess
import textwrap


def _get_command(path, command_directory):
    '''Read a _compile_command file and return the parsed JSON string.
    Args:
      path: The pathlib.Path to _compile_command file.
      command_directory: The directory commands are run from.
    Returns:
      a string to stick in compile_commands.json.
    '''
    with path.open('r') as f:
        contents = f.read().split('\0')
        if len(contents) != 2:
            # Old/incomplete file or something; silently ignore it.
            return None

        command, file = contents
        return textwrap.dedent('''\
        {
          "directory": "%s",
          "command": "%s",
          "file": "%s"
        }''' % (command_directory, command.replace('"', '\\"'), file))


def _get_compile_commands(path, command_directory):
    '''Traverse a path and returns parsed command JSON strings.
    Args:
      path: A directory pathlib.Path to look for _compile_command files under.
      command_directory: The directory commands are run from.
    Yields:
      strings to stick in compile_commands.json.
    '''
    for f in path.iterdir():
        if f.is_dir():
            yield from _get_compile_commands(f, command_directory)
        elif f.name.endswith('_compile_command'):
            command = _get_command(f, command_directory)
            if command:
                yield command


def main(argv):
    source_path = os.path.join(os.path.dirname(__file__), '../..')
    action_outs = os.path.join(
        source_path, 'bazel-bin/../extra_actions',
        'tools/actions/generate_compile_commands_action')

    command_directory = subprocess.check_output(
        ('bazel', 'info', 'execution_root'),
        cwd=source_path).decode('utf-8').rstrip()
    commands = list(
        _get_compile_commands(pathlib.Path(action_outs), command_directory))

    with open(os.path.join(source_path, 'compile_commands.json'), 'w') as f:
        f.write('[')
        for i, command in enumerate(commands):
            f.write(command)

            # Do not add the separator for the last element (it isn't valid
            # JSON).
            if i + 1 != len(commands):
                f.write(', ')
        f.write(']')


if __name__ == '__main__':
    sys.exit(main(sys.argv))
