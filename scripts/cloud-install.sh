#!/usr/bin/env bash
set -euo pipefail
cd /workspace/JerzyBuchlaGenerativeEasel
if [[ ! -x /workspace/toolchain/bin/python ]]; then
  python -m venv /workspace/toolchain
fi
/workspace/toolchain/bin/python -m pip install cmake==3.31.6 ninja==1.11.1.4
if [[ ! -d .cache/JUCE/.git ]]; then
  git clone --no-checkout https://github.com/juce-framework/JUCE.git .cache/JUCE
fi
# Only the disposable dependency checkout may be checked out. Never reset the project checkout.
git -C .cache/JUCE checkout 29396c22c93392d6738e021b83196283d6e4d850
mkdir -p .cache/apt .cache/sysroot /workspace/apt-lists/partial /workspace/apt-cache/archives/partial
cat > .cache/apt/sources.list <<'SOURCES'
deb [signed-by=/usr/share/keyrings/debian-archive-keyring.gpg] https://deb.debian.org/debian trixie main
SOURCES
apt_options=(-o "Dir::Etc::sourcelist=$PWD/.cache/apt/sources.list" -o Dir::Etc::sourceparts=- -o Dir::State::Lists=/workspace/apt-lists -o Dir::Cache=/workspace/apt-cache -o APT::Sandbox::User=agent -o Debug::NoLocking=1)
apt-get "${apt_options[@]}" update
apt-get "${apt_options[@]}" --download-only install -y libasound2-dev libx11-dev libxext-dev libxinerama-dev libxrandr-dev libxcursor-dev libfreetype-dev libfontconfig1-dev libgl-dev xvfb ladspa-sdk
python - <<'PY'
import pathlib, subprocess
root=pathlib.Path('.cache/sysroot').resolve()
for deb in pathlib.Path('/workspace/apt-cache/archives').glob('*.deb'):
    subprocess.run(['dpkg-deb','-x',str(deb),str(root)],check=True)
for p in (root/'usr/lib/x86_64-linux-gnu').glob('*.so'):
    if p.is_symlink() and not p.exists():
        target=pathlib.Path('/usr/lib/x86_64-linux-gnu')/p.readlink().name
        if target.exists():
            p.unlink()
            p.symlink_to(target)
PY
source scripts/cloud-env.sh
cmake --preset core
cmake --build --preset core
ctest --preset core
cmake --preset linux
cmake --build --preset linux
ctest --preset linux

if [[ ! -d .cache/pluginval/.git ]]; then
  git clone https://github.com/Tracktion/pluginval.git .cache/pluginval
fi
git -C .cache/pluginval checkout ed19c2c16b57a6d94db391bea3ef4a80b769d5bf
git -C .cache/pluginval submodule update --init --recursive
cmake -S .cache/pluginval -B build/pluginval -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/pluginval --parallel 2
