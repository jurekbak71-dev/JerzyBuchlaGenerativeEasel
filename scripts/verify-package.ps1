$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
Set-Location $root
$release = 'out/release'
$zip = "$release/JerzyVoltageGarden-0.2.0-Windows-x64.zip"
$installer = "$release/JerzyVoltageGarden-0.2.0-Windows-x64-Setup.exe"
$original = 'build/windows/JerzyVoltageGarden_artefacts/Release/VST3/Jerzy Voltage Garden.vst3/Contents/x86_64-win/Jerzy Voltage Garden.vst3'
$originalHash = (Get-FileHash $original -Algorithm SHA256).Hash
$unzip = Join-Path $env:RUNNER_TEMP "JerzyVoltageGarden-zip-$env:GITHUB_RUN_ID"
Expand-Archive $zip $unzip -Force
$unzipped = Join-Path $unzip 'Jerzy Voltage Garden.vst3/Contents/x86_64-win/Jerzy Voltage Garden.vst3'
if ((Get-FileHash $unzipped -Algorithm SHA256).Hash -ne $originalHash) { throw 'ZIP binary differs from compiled VST3' }
Expand-Archive "$release/JerzyVoltageGarden-0.2.0-Source.zip" (Join-Path $unzip 'source') -Force
foreach ($path in @('source/CMakeLists.txt', 'source/src/Engine.h', 'source/JUCE/LICENSE.md', 'LICENSES/AGPL-3.0.txt')) {
    if (!(Test-Path (Join-Path $unzip $path))) { throw "ZIP missing required source or license: $path" }
}
$installDir = Join-Path $env:RUNNER_TEMP "JerzyVoltageGarden-installed-$env:GITHUB_RUN_ID"
$log = Join-Path $root 'out/ci/installer.log'
$arguments = @('/VERYSILENT', '/SUPPRESSMSGBOXES', '/NORESTART', "/DIR=`"$installDir`"", "/LOG=`"$log`"")
$process = Start-Process -FilePath (Resolve-Path $installer) -ArgumentList $arguments -Wait -PassThru
if ($process.ExitCode -ne 0) { throw "Installer exit code $($process.ExitCode)" }
$installed = Join-Path $installDir 'Jerzy Voltage Garden.vst3/Contents/x86_64-win/Jerzy Voltage Garden.vst3'
if ((Get-FileHash $installed -Algorithm SHA256).Hash -ne $originalHash) { throw 'Installed VST3 differs from compiled binary' }
$uninstaller = Join-Path $env:ProgramData 'Jerzy Audio/Jerzy Voltage Garden/Uninstall/unins000.exe'
if (!(Test-Path $uninstaller)) { throw 'Uninstaller missing' }
$process = Start-Process -FilePath $uninstaller -ArgumentList '/VERYSILENT /SUPPRESSMSGBOXES /NORESTART' -Wait -PassThru
if ($process.ExitCode -ne 0) { throw "Uninstaller exit code $($process.ExitCode)" }
if (Test-Path $installed) { throw 'Uninstaller left the VST3 binary installed' }
Write-Output 'PASS: ZIP content and corresponding source verified; installer and uninstaller executed; binary SHA256 matches.'
