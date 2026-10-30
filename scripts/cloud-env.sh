#!/usr/bin/env bash
# Source this file from repository root; no credentials are stored here.
export PATH="/workspace/toolchain/bin:$PATH"
if [[ -d "$PWD/.cache/sysroot/usr/include" ]]; then
  export PKG_CONFIG_SYSROOT_DIR="$PWD/.cache/sysroot"
  export PKG_CONFIG_LIBDIR="$PWD/.cache/sysroot/usr/lib/x86_64-linux-gnu/pkgconfig:$PWD/.cache/sysroot/usr/share/pkgconfig:/usr/lib/x86_64-linux-gnu/pkgconfig"
  export CPLUS_INCLUDE_PATH="$PWD/.cache/sysroot/usr/include:$PWD/.cache/sysroot/usr/include/freetype2"
  export C_INCLUDE_PATH="$CPLUS_INCLUDE_PATH"
  export LIBRARY_PATH="$PWD/.cache/sysroot/usr/lib/x86_64-linux-gnu"
fi
