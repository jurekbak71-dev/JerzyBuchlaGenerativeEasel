$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root
# These directories contain only generated packaging output.
if (Test-Path out/package) { Remove-Item out/package -Recurse -Force }
New-Item out/package, out/release -ItemType Directory -Force | Out-Null
$bundle = 'build/windows/VoltageGarden_artefacts/Release/VST3/Voltage Garden.vst3'
if (!(Test-Path "$bundle/Contents/x86_64-win/Voltage Garden.vst3")) { throw 'Missing compiled Windows x64 VST3' }
$binary = [System.IO.File]::ReadAllBytes((Resolve-Path "$bundle/Contents/x86_64-win/Voltage Garden.vst3"))
$peOffset = [BitConverter]::ToInt32($binary, 0x3c)
if ($peOffset -lt 0 -or $peOffset + 6 -gt $binary.Length -or [BitConverter]::ToUInt32($binary,$peOffset) -ne 0x4550 -or [BitConverter]::ToUInt16($binary,$peOffset+4) -ne 0x8664) { throw 'Bundle is not an x64 PE binary' }
Copy-Item $bundle out/package -Recurse -Force
Copy-Item docs,LICENSES,presets out/package -Recurse -Force
Copy-Item README.md,LICENSE out/package
$commit = git rev-parse HEAD
if ($LASTEXITCODE -ne 0) { throw 'Missing build identity' }
"Source commit: $commit`nMSVC x64 / Release / JUCE 8.0.12" | Set-Content out/package/BUILD.txt
Get-ChildItem "$bundle/Contents" -Recurse -File | Get-FileHash -Algorithm SHA256 | ForEach-Object { "$($_.Hash.ToLower())  $($_.Path.Substring((Resolve-Path $bundle).Path.Length + 1))" } | Set-Content out/package/VST3-SHA256SUMS.txt
$source = 'out/package/source'
New-Item $source -ItemType Directory -Force | Out-Null
git archive --format=zip -o out/source.zip HEAD
if ($LASTEXITCODE -ne 0) { throw 'Corresponding source archive failed' }
Expand-Archive out/source.zip $source -Force
# Include exact JUCE dependency sources; no binaries without corresponding dependency source.
Copy-Item build/windows/_deps/juce-src "$source/JUCE" -Recurse -Force
Get-ChildItem "$source/JUCE" -Directory -Hidden -Filter '.git' | Remove-Item -Recurse -Force
Compress-Archive out/package/* out/release/VoltageGarden-0.1.0-Windows-x64.zip -Force
$iscc = "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe"
if (!(Test-Path $iscc)) { throw 'Inno Setup 6 not installed on runner' }
& $iscc packaging/VoltageGarden.iss
if ($LASTEXITCODE -ne 0) { throw 'Installer failed' }
Get-ChildItem out/release -File | Where-Object Name -ne 'SHA256SUMS.txt' | Get-FileHash -Algorithm SHA256 | ForEach-Object { "$($_.Hash.ToLower())  $(Split-Path $_.Path -Leaf)" } | Set-Content out/release/SHA256SUMS.txt
