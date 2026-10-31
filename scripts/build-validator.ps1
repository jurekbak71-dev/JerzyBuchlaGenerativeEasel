$ErrorActionPreference = 'Stop'
if (!(Test-Path out/pluginval/.git)) {
    git clone https://github.com/Tracktion/pluginval.git out/pluginval
    if ($LASTEXITCODE -ne 0) { throw 'pluginval clone failed' }
}
git -C out/pluginval checkout ed19c2c16b57a6d94db391bea3ef4a80b769d5bf
if ($LASTEXITCODE -ne 0) { throw 'pluginval checkout failed' }
git -C out/pluginval submodule update --init --recursive
if ($LASTEXITCODE -ne 0) { throw 'pluginval dependencies failed' }
$compat = (Resolve-Path scripts/pluginval-compat.cmake).Path
cmake -S out/pluginval -B out/pluginval-build -G "Ninja Multi-Config" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl "-DCMAKE_PROJECT_pluginval_INCLUDE=$compat"
if ($LASTEXITCODE -ne 0) { throw 'pluginval configure failed' }
cmake --build out/pluginval-build --config Release --parallel 2
if ($LASTEXITCODE -ne 0) { throw 'pluginval build failed' }
