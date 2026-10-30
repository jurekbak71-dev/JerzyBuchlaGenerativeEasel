#!/usr/bin/env python3
"""Record compiler/test output while preserving the exact subprocess exit status."""
import pathlib
import subprocess
import sys


def main():
    if len(sys.argv) < 3:
        raise SystemExit('usage: ci-command.py LOG COMMAND [ARGS...]')
    log = pathlib.Path(sys.argv[1])
    log.parent.mkdir(parents=True, exist_ok=True)
    with log.open('wb') as output:
        process = subprocess.Popen(sys.argv[2:], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        for chunk in iter(lambda: process.stdout.read(4096), b''):
            output.write(chunk)
            output.flush()
            sys.stdout.buffer.write(chunk)
            sys.stdout.buffer.flush()
        return process.wait()


if __name__ == '__main__':
    sys.exit(main())
