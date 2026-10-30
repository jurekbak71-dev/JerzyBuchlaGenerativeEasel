# Instrukcja dla Codexa — Jerzy Buchla Generative Easel

## Misja
Dostarcz produkcyjny, stabilny instrument VST3 Windows x64 zgodny z docs/MASTER_PROMPT.md. Zanim uznasz zadanie za zakończone, skompiluj, uruchom testy i udostępnij rzeczywisty VST3 w artefaktach CI, wraz z instalatorem i ZIP.

## Zasady pracy
- Użyj C++20, JUCE i CMake, po zweryfikowaniu aktualnych zgodnych wersji/licencji. Budowanie i testy na windows-latest / MSVC.
- Realizuj iteracyjnie, z działającym przyrostem i testami po każdym etapie. Nie zostawiaj samych atrap GUI ani symulowanego dźwięku.
- Referencyjny wygląd GUI został zaakceptowany w rozmowie. Rzeczywisty screenshot z działającego GUI musi zostać wygenerowany i przedstawiony do oceny zgodności. Nie zatrzymuj pracy wyłącznie dlatego, że pierwotna koncepcja graficzna jest już zatwierdzona; zmiany istotnie odbiegające od wzorca wymagają zgody.
- Obowiązkowe patchowanie przewodami na panelu: audio, CV, gate, trigger, clock; patch graph działa w DSP, nie tylko wizualnie.
- Działaj tylko w tym repozytorium. Repozytoria referencyjne czytaj bez zmian. Nie ujawniaj kluczy i sekretów.
- Research z aktualnych źródeł: GitHub, Buchla, Steinberg VST3, JUCE, FL Studio, dokumentacja DSP, forum i publikacje. Zapisuj daty, licencje, odnośniki i niepewności.
- Nie kopiuj grafiki, znaków towarowych ani kodu bez odpowiedniej licencji.
- Jeśli nie masz dostępu do plików PDF/PNG z rozmowy, podaj brak precyzyjnie, kontynuuj z publicznymi źródłami i nie udawaj że je analizowałeś.
- Wtyczka musi działać w audio renderze offline tak samo jak w odtwarzaniu, przy tym samym stanie i timeline; bez samoczynnego startu generatora w trybie MIDI.
- Nie ogłaszaj zaliczenia testów hostowych bez ich rzeczywistego wykonania.
- Dokumentuj postęp w docs/IMPLEMENTATION_STATUS.md (utwórz podczas pracy): zrobione, dowody, testy, ograniczenia, linki do artefaktów.
- Zachowaj stabilność realtime: bez heap allocation, locks, I/O na callback audio; poprawne stop/reset, NaN/Inf, denormals, watchdog/cpu protection.
- Nie zmieniaj struktury zewnętrznych projektów, uprawnień ani udostępniania bez odrębnej zgody.

## Kolejność
1. Audit źródeł i repozytoriów; architektura/testy.
2. Rdzeń DSP + modulacja/patch graph + najprostszy dźwięk MIDI.
3. Generative engine i prawidłowe zachowanie transportu/FL.
4. GUI o zatwierdzonym charakterze, screenshoty z działającej wtyczki.
5. Integracja, hardening, testy, release .vst3 + instalator + ZIP.

## Definition of done
Przestrzegaj docs/ACCEPTANCE.md. Nie kończ na kodzie ani samym zielonym workflow.
