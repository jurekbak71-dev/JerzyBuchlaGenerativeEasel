# Jerzy Buchla Generative Easel

Autorski instrument VST3 dla Windows x64 inspirowany filozofią Buchla Music Easel / serii 100 i 200. **To nie jest oficjalny produkt Buchla ani deklaracja wiernej emulacji 208C.**

## Status (2026-10-31)
- **VST3 Windows x64 został skompilowany przez MSVC i przeszedł CTest/GUI; Linux ma również test pluginval. Pełny produkt nadal jest nieukończony.** Windows pluginval, instalator i ZIP wymagają zakończenia nowego uruchomienia CI — odnośnik pobierania nie jest jeszcze potwierdzony.
- Kierunek GUI z wizualizacji w rozmowie zaakceptowany; interaktywny patching wirtualnymi kabelkami jest wymogiem obowiązkowym.
- Autorska nazwa UI: **Voltage Garden**. Działają audio/MIDI, podstawowy generator, stan hosta i rzeczywisty graf kabli. Pozostające wymagania i blokady: [IMPLEMENTATION_STATUS](docs/IMPLEMENTATION_STATUS.md).
- Budowanie: [BUILD](docs/BUILD.md); obsługa: [instrukcja PL](docs/MANUAL_PL.md); dowody: [screenshots](docs/screenshots/), [testy](docs/evidence/).
- Docelowe wydanie Windows wymaga rzeczywistej kompilacji MSVC i testów hostowych; sam workflow nie jest dostarczonym produktem.

## Dla Codexa
1. Przeczytaj [AGENTS.md](AGENTS.md) oraz [docs/MASTER_PROMPT.md](docs/MASTER_PROMPT.md).
2. Następnie [docs/ACCEPTANCE.md](docs/ACCEPTANCE.md), [docs/GUI_SPEC.md](docs/GUI_SPEC.md), [docs/SOURCES.md](docs/SOURCES.md).
3. Użyj [docs/EXISTING_REPOS.md](docs/EXISTING_REPOS.md) jako katalogu do sprawdzenia istniejącego kodu.
4. Przed implementacją sprawdź prawdziwy stan repozytoriów, licencje, zależności i dostępne narzędzia.

## Ważne
Referencyjne grafiki i PDF pochodzą z rozmowy ChatGPT; **nie zostały automatycznie przeniesione do GitHub**. Lista i wymagany sposób ich dołączenia: [references/README.md](references/README.md). Nie twierdź, że je masz, dopóki nie zweryfikujesz plików.

**Definicja ukończenia:** istnieją zweryfikowane pliki .vst3, paczka ZIP, instalator, testy i adresy artefaktów, a nie sam kod źródłowy.
