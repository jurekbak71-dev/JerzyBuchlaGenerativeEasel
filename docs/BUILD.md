# Budowanie

Windows x64: CMake ≥3.25 (test lokalny: 3.31.6), MSVC VS2022, C++20. `cmake --preset windows`, `cmake --build --preset windows`, `ctest --preset windows`. Przypięty JUCE 8.0.12: commit `29396c22c93392d6738e021b83196283d6e4d850`; jego dołączone nagłówki SDK VST3 są częścią tej samej rewizji. Bez płatnego SDK. Nie pobiera się VST2, ASIO SDK ani oficjalnej grafiki.

Workflow `.github/workflows/windows.yml` buduje również pluginval 1.0.4 z przypiętej rewizji, uruchamia walidację VST3 i dopiero potem pakowanie. Inno Setup 6 musi być zainstalowany na runnerze. `scripts/package.ps1` wymaga prawdziwego pliku PE wewnątrz `Contents/x86_64-win`; nie produkuje ZIP z atrapą. Instalator trafia do wspólnego katalogu VST3, ZIP zachowuje bundle. Podpis Authenticode i FL Studio nie są zapewnione przez CI.

Linux cloud (Debian 13, konto agent): `bash scripts/cloud-install.sh`. Skrypt nie wymaga roota; podpisane pakiety dev rozpakowuje do `.cache/sysroot`, narzędzia Python do `/workspace/toolchain`. Nie usuwa/nie resetuje repozytorium projektu. Korzysta z istniejącego checkoutu; nie twórz worktree. W przyszłych sesjach uruchom z katalogu repo `source scripts/cloud-env.sh` przed CMake. Brak WebKit jest oczekiwany: przeglądarka i curl JUCE są wyłączone.

Testy bez JUCE/sieci: `cmake --preset core`, `cmake --build --preset core`, `ctest --preset core`. GUI i integracja: `cmake --preset linux`, `cmake --build --preset linux`, `ctest --preset linux`. Ekran wirtualny: `.cache/sysroot/usr/bin/Xvfb :99 -screen 0 1600x1000x24 -nolisten tcp`; następnie `DISPLAY=:99 'build/linux/garden_integration_artefacts/Release/garden_integration' --screenshots`. Przed snapshotami potrzebny jest także menedżer okien: `DISPLAY=:99 xfwm4 --replace --compositor=off`. Bez niego JUCE/X11 zgłaszał BadAtom; naprawiono konfigurację środowiska. Pełny test uruchamia `bash scripts/gui-test.sh`. Screenshoty to wynik rzeczywistego komponentu edytora po addToDesktop, a nie renderer HTML czy obraz wygenerowany przez AI.

Nie publikuj wydania produkcyjnego, dopóki brakujące kryteria w IMPLEMENTATION_STATUS nie zostaną rzeczywiście spełnione.
