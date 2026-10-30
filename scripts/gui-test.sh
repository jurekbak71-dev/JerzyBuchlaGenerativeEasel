#!/usr/bin/env bash
set -euo pipefail
cd /workspace/JerzyBuchlaGenerativeEasel
source scripts/cloud-env.sh
export DISPLAY=${DISPLAY:-:99}
mkdir -p out/gui out/tests
if ! xdpyinfo >/dev/null 2>&1; then
  mkdir -p /tmp/.X11-unix
  .cache/sysroot/usr/bin/Xvfb "$DISPLAY" -screen 0 1600x1000x24 -nolisten tcp >out/gui/xvfb.log 2>&1 &
  echo $! >out/gui/xvfb.pid
  for i in {1..30}; do if xdpyinfo >/dev/null 2>&1; then break; fi; sleep 0.1; done
  xdpyinfo >/dev/null
fi
if ! xprop -root _NET_SUPPORTING_WM_CHECK 2>/dev/null | rg -q 'window id'; then
  xfwm4 --replace --compositor=off >out/gui/wm.log 2>&1 &
  echo $! >out/gui/wm.pid
  for i in {1..30}; do if xprop -root _NET_SUPPORTING_WM_CHECK 2>/dev/null | rg -q 'window id'; then break; fi; sleep 0.1; done
  xprop -root _NET_SUPPORTING_WM_CHECK | rg 'window id'
fi
build/linux/garden_integration_artefacts/Release/garden_integration --screenshots >out/tests/gui.txt 2>&1
build/pluginval/pluginval_artefacts/Release/pluginval --strictness-level 5 --validate "$PWD/build/linux/VoltageGarden_artefacts/Release/VST3/Voltage Garden.vst3" --output-dir "$PWD/out/validation" >out/tests/pluginval.txt 2>&1
