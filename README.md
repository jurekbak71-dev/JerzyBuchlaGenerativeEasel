# Jerzy Buchla Generative Easel

Autorski instrument VST3 dla Windows x64 inspirowany filozofią Buchla Music Easel / serii 100 i 200. **To nie jest oficjalny produkt Buchla ani deklaracja wiernej emulacji 208C.**

## Status (2026-11-01)

Działająca wersja rozwojowa **Jerzy Voltage Garden 0.1.0** dla Windows x64:

- [Pobierz VST3 ZIP — 3,3 MB](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/d15f8c232913fcce86a18ae91f58b12ee97bfa7b/JerzyVoltageGarden-0.1.0-Windows-x64.zip)
- [Pobierz instalator — 4,7 MB](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/d15f8c232913fcce86a18ae91f58b12ee97bfa7b/JerzyVoltageGarden-0.1.0-Windows-x64-Setup.exe)
- [Pełne źródła z JUCE — 27,8 MB](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/d15f8c232913fcce86a18ae91f58b12ee97bfa7b/JerzyVoltageGarden-0.1.0-Source.zip)

Pliki pobrano i zweryfikowano poza CI. Windows: MSVC, CTest, pluginval, ładowanie VST3, audio MIDI, rzeczywiste GUI, WAV oraz wykonanie instalatora i uninstallera przeszły testy. [Dowody i ograniczenia](docs/IMPLEMENTATION_STATUS.md), [budowanie](docs/BUILD.md), [instrukcja PL](docs/MANUAL_PL.md).

To nadal nie jest pełna realizacja MASTER_PROMPT: m.in. brak testów FL Studio, oryginalnej zatwierdzonej makiety oraz części zaawansowanych funkcji generatora. Instalator jest niepodpisany. Interaktywny patching wpływa na rzeczywisty DSP.

## Dla Codexa
1. Przeczytaj [AGENTS.md](AGENTS.md) oraz [docs/MASTER_PROMPT.md](docs/MASTER_PROMPT.md).
2. Następnie [docs/ACCEPTANCE.md](docs/ACCEPTANCE.md), [docs/GUI_SPEC.md](docs/GUI_SPEC.md), [docs/SOURCES.md](docs/SOURCES.md).
3. Użyj [docs/EXISTING_REPOS.md](docs/EXISTING_REPOS.md) jako katalogu do sprawdzenia istniejącego kodu.
4. Przed implementacją sprawdź prawdziwy stan repozytoriów, licencje, zależności i dostępne narzędzia.

## Ważne
Referencyjne grafiki i PDF pochodzą z rozmowy ChatGPT; **nie zostały automatycznie przeniesione do GitHub**. Lista i wymagany sposób ich dołączenia: [references/README.md](references/README.md). Nie twierdź, że je masz, dopóki nie zweryfikujesz plików.

**Definicja ukończenia:** istnieją zweryfikowane pliki .vst3, paczka ZIP, instalator, testy i adresy artefaktów, a nie sam kod źródłowy.
