#!/usr/bin/env python3
"""Record compiler/test output while preserving the exact subprocess exit status."""
import os
import pathlib
import subprocess
import sys
import threading


ROOT = pathlib.Path(__file__).resolve().parents[1]


def progress_enabled(log, environ):
    return (environ.get('GITHUB_ACTIONS') == 'true'
            and environ.get('GARDEN_CI_PROGRESS') == '1'
            and environ.get('GITHUB_EVENT_NAME') != 'pull_request'
            and log.resolve().parent == ROOT / 'out/ci')


def publish_progress(stop, log):
    while not stop.wait(25):
        try:
            # Ordinary Git authentication stays on the runner. No API token handling.
            with (log.parent / 'report-publish.log').open('ab') as output:
                subprocess.run([sys.executable, str(ROOT / 'scripts/publish-ci-status.py'),
                                'progress/' + log.stem], cwd=ROOT, stdout=output,
                               stderr=subprocess.STDOUT, timeout=25, check=False)
        except (OSError, subprocess.TimeoutExpired):
            # Evidence publication must never turn a successful compiler result into failure.
            pass


def main():
    if len(sys.argv) < 3:
        raise SystemExit('usage: ci-command.py LOG COMMAND [ARGS...]')
    log = pathlib.Path(sys.argv[1])
    log.parent.mkdir(parents=True, exist_ok=True)
    stop = threading.Event()
    reporter = None
    if progress_enabled(log, os.environ):
        reporter = threading.Thread(target=publish_progress, args=(stop, log), daemon=True)
        reporter.start()
    try:
        return run_logged(log)
    finally:
        stop.set()
        if reporter is not None:
            reporter.join()


def run_logged(log):
    with log.open('wb') as output:
        try:
            process = subprocess.Popen(sys.argv[2:], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        except OSError as error:
            message = ('failed to start command: ' + str(error) + '\n').encode('utf-8', errors='replace')
            output.write(message)
            sys.stderr.buffer.write(message)
            return 127
        for chunk in iter(lambda: process.stdout.read1(4096), b''):
            output.write(chunk)
            output.flush()
            sys.stdout.buffer.write(chunk)
            sys.stdout.buffer.flush()
        return process.wait()


if __name__ == '__main__':
    sys.exit(main())
