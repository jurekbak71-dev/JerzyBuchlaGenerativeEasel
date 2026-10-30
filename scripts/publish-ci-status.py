#!/usr/bin/env python3
"""Publish bounded build evidence to an isolated branch using the runner's Git auth.

No token is read, copied or put in a remote URL. GitHub Actions' checkout credential
is used normally by native Git. No worktree, force push or source checkout reset.
"""
import datetime
import json
import os
import pathlib
import subprocess
import sys

BRANCH = 'codex/windows-build-status'
PRODUCER = 'voltage-garden-windows-ci'


def git(*args, data=None, check=True):
    result = subprocess.run(['git', *args], input=data, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    if check and result.returncode:
        # Never dump Git config, credentials or credential-bearing output.
        raise RuntimeError('Git operation failed: ' + args[0])
    return result


def build_report(phase, environ):
    return {
        'schema': 1, 'producer': PRODUCER, 'phase': phase,
        'head_sha': environ.get('GITHUB_SHA'),
        'run_id': environ.get('GITHUB_RUN_ID'),
        'run_attempt': environ.get('GITHUB_RUN_ATTEMPT'),
        'run_url': '{}/{}/actions/runs/{}'.format(
            environ.get('GITHUB_SERVER_URL', 'https://github.com'),
            environ.get('GITHUB_REPOSITORY', ''), environ.get('GITHUB_RUN_ID', '')),
        'artifact_url': environ.get('REPORT_ARTIFACT_URL', ''),
        'artifact_id': environ.get('REPORT_ARTIFACT_ID', ''),
        'steps': json.loads(environ.get('REPORT_STEPS') or '{}'),
        'updated_at': datetime.datetime.now(datetime.timezone.utc).isoformat(),
    }


def evidence_files(root):
    result = {}
    # Only known build output, never process environment or arbitrary runner files.
    for pattern in ('out/ci/*.log', 'out/tests/windows.xml', 'out/release/SHA256SUMS.txt',
                    'build/windows/Testing/Temporary/LastTest.log'):
        for path in sorted(root.glob(pattern)):
            if path.is_file():
                # Bound each record; tail retains compiler errors and test summaries.
                with path.open('rb') as stream:
                    size = path.stat().st_size
                    stream.seek(max(0, size - 256_000))
                    data = stream.read(256_000)
                result['evidence-' + path.name] = data
    return result


def main():
    if os.environ.get('GITHUB_ACTIONS') != 'true':
        raise RuntimeError('Publication is restricted to the GitHub Actions runner')
    if os.environ.get('GITHUB_EVENT_NAME') == 'pull_request':
        raise RuntimeError('Pull requests cannot publish status branches')
    phase = sys.argv[1]
    report = build_report(phase, os.environ)
    parent = None
    remote = git('ls-remote', 'origin', 'refs/heads/' + BRANCH).stdout.strip()
    if remote:
        git('fetch', '--no-tags', 'origin', 'refs/heads/' + BRANCH)
        parent = git('rev-parse', 'FETCH_HEAD').stdout.decode().strip()
        old = git('show', parent + ':status.json')
        if json.loads(old.stdout).get('producer') != PRODUCER:
            raise RuntimeError('Status branch is not owned by this automation; refusing replacement')
    files = evidence_files(pathlib.Path('.'))
    files['status.json'] = (json.dumps(report, indent=2) + '\n').encode()
    tree_lines = []
    for name, content in sorted(files.items()):
        oid = git('hash-object', '-w', '--stdin', data=content).stdout.decode().strip()
        tree_lines.append('100644 blob {}\t{}\n'.format(oid, name))
    tree = git('mktree', data=''.join(tree_lines).encode()).stdout.decode().strip()
    git('config', 'user.name', 'github-actions[bot]')
    git('config', 'user.email', '41898282+github-actions[bot]@users.noreply.github.com')
    args = ['commit-tree', tree]
    if parent:
        args.extend(['-p', parent])
    args.extend(['-m', 'Windows {} for {} [skip ci]'.format(phase, report['head_sha'])])
    commit = git(*args).stdout.decode().strip()
    git('push', 'origin', commit + ':refs/heads/' + BRANCH)
    print('Published Windows build status:', phase, report['run_url'])


if __name__ == '__main__':
    main()
