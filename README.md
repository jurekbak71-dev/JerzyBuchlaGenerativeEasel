# Jerzy Buchla Generative Easel

Autorski instrument VST3 dla Windows x64 inspirowany filozofią Buchla Music Easel / serii 100 i 200. **To nie jest oficjalny produkt Buchla ani deklaracja wiernej emulacji 208C.**

## Status (2026-11-01)

**Jerzy Voltage Garden 0.2.0** ma GUI przebudowane według zdjęcia zatwierdzonego przez użytkownika: jasny panel 20 suwaków, zwarta sekcja gałek, niebiesko-czerwony kontroler 29 kontaktów oraz działające kable DSP.

- [Pobierz Windows x64 VST3 ZIP — 4,2 MB](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/JerzyVoltageGarden-0.2.0-Windows-x64.zip)
- [Pobierz instalator — 5,5 MB](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/JerzyVoltageGarden-0.2.0-Windows-x64-Setup.exe)
- [Źródła z JUCE — 28,7 MB](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/JerzyVoltageGarden-0.2.0-Source.zip)
- [Obejrzyj rzeczywiste GUI Windows](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/Jerzy-screenshot-Jerzy-loaded-vst3.png)

Pliki pobrano i zweryfikowano poza CI. Windows zaliczył CTest, pluginval, ładowanie VST3, MIDI/audio, GUI, WAV oraz instalację i odinstalowanie. [Dowody i ograniczenia](docs/IMPLEMENTATION_STATUS.md), [porównanie z referencją](docs/GUI_REFERENCE_QA.md), [instrukcja PL](docs/MANUAL_PL.md).

Silnik pozostaje wersją rozwojową z brakującymi funkcjami pełnego MASTER_PROMPT. FL Studio nie było testowane. Instalator jest niepodpisany.

## Dla Codexa
1. Przeczytaj [AGENTS.md](AGENTS.md) oraz [docs/MASTER_PROMPT.md](docs/MASTER_PROMPT.md).
2. Następnie [docs/ACCEPTANCE.md](docs/ACCEPTANCE.md), [docs/GUI_SPEC.md](docs/GUI_SPEC.md), [docs/SOURCES.md](docs/SOURCES.md).
3. Użyj [docs/EXISTING_REPOS.md](docs/EXISTING_REPOS.md) jako katalogu do sprawdzenia istniejącego kodu.
4. Przed implementacją sprawdź prawdziwy stan repozytoriów, licencje, zależności i dostępne narzędzia.

## Ważne
Referencyjne grafiki i PDF pochodzą z rozmowy ChatGPT; **nie zostały automatycznie przeniesione do GitHub**. Lista i wymagany sposób ich dołączenia: [references/README.md](references/README.md). Nie twierdź, że je masz, dopóki nie zweryfikujesz plików.

**Definicja ukończenia:** istnieją zweryfikowane pliki .vst3, paczka ZIP, instalator, testy i adresy artefaktów, a nie sam kod źródłowy.
