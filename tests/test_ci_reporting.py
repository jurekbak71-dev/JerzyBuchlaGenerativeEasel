"""Integration checks use local temporary Git remotes; never contact GitHub."""
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PUBLISHER = ROOT / 'scripts/publish-ci-status.py'
WRAPPER = ROOT / 'scripts/ci-command.py'


class ReportingTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary.name)
        self.remote = self.root / 'remote.git'
        self.work = self.root / 'work'
        self.work.mkdir()
        self.git('init', '--bare', str(self.remote))
        self.git('init', '-b', 'main')
        self.git('config', 'user.name', 'CI test')
        self.git('config', 'user.email', 'ci-test@example.invalid')
        (self.work / 'source.txt').write_text('keep this source\n')
        self.git('add', 'source.txt')
        self.git('commit', '-m', 'source baseline')
        self.sha = self.git('rev-parse', 'HEAD').strip()
        self.git('remote', 'add', 'origin', str(self.remote))
        self.git('push', 'origin', 'main')
        self.env = dict(os.environ, GITHUB_ACTIONS='true', GITHUB_EVENT_NAME='push',
                        GITHUB_SHA=self.sha, GITHUB_RUN_ID='123', GITHUB_RUN_ATTEMPT='1',
                        GITHUB_REPOSITORY='test/project', REPORT_STEPS='{}')

    def tearDown(self):
        self.temporary.cleanup()

    def git(self, *args):
        return subprocess.check_output(['git', *args], cwd=self.work,
                                       stderr=subprocess.PIPE, text=True)

    def publish(self, phase):
        return subprocess.run([sys.executable, str(PUBLISHER), phase], cwd=self.work,
                              env=self.env, capture_output=True, text=True)

    def read_report(self):
        return json.loads(self.git('--git-dir=' + str(self.remote), 'show',
                                   'refs/heads/codex/windows-build-status:status.json'))

    def test_publication_preserves_checkout_and_appends_without_force(self):
        (self.work / 'source.txt').write_text('uncommitted user change\n')
        before = self.git('status', '--porcelain')
        for phase in ('running', 'success'):
            result = self.publish(phase)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(self.read_report()['phase'], phase)
            self.assertEqual(self.read_report()['head_sha'], self.sha)
            self.assertEqual(self.git('rev-parse', 'HEAD').strip(), self.sha)
            self.assertEqual(self.git('status', '--porcelain'), before)
        self.assertEqual(self.git('--git-dir=' + str(self.remote), 'rev-list', '--count',
                                  'refs/heads/codex/windows-build-status').strip(), '2')

    def test_evidence_is_bounded_and_only_expected_outputs_are_copied(self):
        directory = self.work / 'out/ci'
        directory.mkdir(parents=True)
        (directory / 'build.log').write_bytes(b'x' * 300_000 + b'final error')
        (directory / 'not-a-log.secret').write_text('must not be included')
        result = self.publish('failure')
        self.assertEqual(result.returncode, 0, result.stderr)
        files = self.git('--git-dir=' + str(self.remote), 'ls-tree', '--name-only',
                         'refs/heads/codex/windows-build-status')
        self.assertNotIn('secret', files)
        evidence = self.git('--git-dir=' + str(self.remote), 'show',
                            'refs/heads/codex/windows-build-status:evidence-build.log')
        self.assertEqual(len(evidence), 256_000)
        self.assertTrue(evidence.endswith('final error'))

    def test_unowned_branch_is_preserved(self):
        self.git('push', 'origin', 'HEAD:refs/heads/codex/windows-build-status')
        result = self.publish('success')
        self.assertNotEqual(result.returncode, 0)
        ref = self.git('--git-dir=' + str(self.remote), 'rev-parse',
                       'refs/heads/codex/windows-build-status').strip()
        self.assertEqual(ref, self.sha)

    def test_pr_cannot_publish(self):
        self.env['GITHUB_EVENT_NAME'] = 'pull_request'
        self.assertNotEqual(self.publish('running').returncode, 0)
        refs = self.git('--git-dir=' + str(self.remote), 'show-ref')
        self.assertNotIn('windows-build-status', refs)

    def test_progress_is_restricted_to_opted_in_runner_output(self):
        spec = importlib.util.spec_from_file_location('ci_command', WRAPPER)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        env = dict(GITHUB_ACTIONS='true', GARDEN_CI_PROGRESS='1', GITHUB_EVENT_NAME='push')
        self.assertTrue(module.progress_enabled(ROOT / 'out/ci/build.log', env))
        self.assertFalse(module.progress_enabled(self.work / 'build.log', env))
        env['GITHUB_EVENT_NAME'] = 'pull_request'
        self.assertFalse(module.progress_enabled(ROOT / 'out/ci/build.log', env))
        self.assertFalse(module.progress_enabled(ROOT / 'out/ci/build.log', {}))

    def test_missing_executable_is_reported_in_log(self):
        log = self.work / 'missing.log'
        result = subprocess.run([sys.executable, str(WRAPPER), str(log),
                                 'not-a-real-voltage-garden-executable'], capture_output=True)
        self.assertEqual(result.returncode, 127)
        self.assertIn(b'failed to start command', log.read_bytes())

    def test_command_wrapper_preserves_failure_and_output(self):
        log = self.work / 'command.log'
        result = subprocess.run([sys.executable, str(WRAPPER), str(log), sys.executable,
                                 '-c', "print('real failure'); raise SystemExit(7)"],
                                capture_output=True)
        self.assertEqual(result.returncode, 7)
        self.assertIn(b'real failure', log.read_bytes())


if __name__ == '__main__':
    unittest.main()
