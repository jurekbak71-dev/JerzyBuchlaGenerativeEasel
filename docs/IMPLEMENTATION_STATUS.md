# Przebudowa GUI 0.2 — w toku

Nowa docelowa fotografia została potwierdzona przez użytkownika 2026-11-01. Przebudowano układ według proporcji sprzętu: górna płyta suwaków i dolny kontroler dotykowy. Wstępne testy Linux zaliczyły rzeczywisty routing po zmianie pozycji gniazd, działanie długiego suwaka i zsynchronizowanej gałki, dotykową klawiaturę i ładowanie gotowego VST3. Nowa wersja 0.2.0 wymaga jeszcze pełnej kompilacji i dostawy Windows. Linki 0.1 poniżej dotyczą poprzedniego GUI, nie nowej kompozycji.

---

# Jerzy Voltage Garden 0.1.0 — stan 2026-11-01

Dostępne są rzeczywiste, pobrane i sprawdzone pliki Windows x64. **Jest to działająca wersja rozwojowa, a nie pełna realizacja MASTER_PROMPT.** Nie deklarujemy testów FL Studio ani zgodności pikselowej z nieobecną makietą.

## Pobieranie

- [VST3 ZIP — 3 313 722 bajty](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/d15f8c232913fcce86a18ae91f58b12ee97bfa7b/JerzyVoltageGarden-0.1.0-Windows-x64.zip)
- [Instalator Windows — 4 728 081 bajtów](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/d15f8c232913fcce86a18ae91f58b12ee97bfa7b/JerzyVoltageGarden-0.1.0-Windows-x64-Setup.exe)
- [Pełne źródła z JUCE — 27 825 436 bajtów](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/d15f8c232913fcce86a18ae91f58b12ee97bfa7b/JerzyVoltageGarden-0.1.0-Source.zip)
- [Artefakt Actions](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/actions/runs/41322069670/artifacts/13089862738)

Linki bezpośrednie zostały sprawdzone przez HTTP: odpowiedź 206 i sygnatury ZIP/EXE. Są przypięte do niezmiennego commitu dostawy `d15f8c232913fcce86a18ae91f58b12ee97bfa7b`, nie do ruchomej gałęzi. Kod kompilacji: `cc845245201a77e420f6d0f976758c3ec550a189`. [Uruchomienie Windows](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/actions/runs/41322069670). Późniejszy commit dokumentacyjny nie oznacza nowej kompilacji aplikacji.

## Co poprawiono

Nazwa produktu, projektu CMake, bundla i paczek zaczyna się od Jerzy. Poprawiono kontrast strzałek list i aktywnych przycisków; dodano zmianę oktawy oraz widoczność kabli bez zmiany DSP. Przesuwanie po klawiaturze zwalnia poprzednią nutę i gra nową; opuszczenie klawiatury zwalnia nutę. Zachowano monofoniczny silnik i format stanu hosta.

Dodano test ładowania rzeczywistego VST3 i obraz jego natywnego okna. Przechwytywanie Windows używa ekranu, ponieważ DC nadrzędnego okna odcinało osadzone dziecko VST3. ZIP binarny oddzielono od źródeł; skopiowana historia Git jest usuwana niezależnie od atrybutu Hidden. Dostawa przez Git działa bez API Actions.

## Dowody

| Sprawdzenie | Wynik |
|---|---|
| Windows x64 MSVC 19.51 / JUCE 8.0.12 | PASS — kompilacja VST3, standalone i testów |
| Windows CTest | PASS 2/2; 45 sprawdzeń rdzenia |
| Windows pluginval 1.0.4, strictness 5 | PASS — rzeczywisty bundle |
| Osobny host JUCE ładuje VST3 | PASS — skanowanie, instancja, MIDI idle silent, audio po Note On, zapis stanu, utworzenie i zamknięcie edytora |
| GUI Windows | PASS — clean, patched, generative, expert; drag zmienia DSP, slide zmienia nutę, wyjście zwalnia klawisz |
| WAV offline | PASS — odczyt pliku stereo 48 kHz/24 bit, zgodność próbek; render niezależny od bloku 32/1024 |
| ZIP, instalator, uninstaller | PASS — wykonane na Windows, SHA256 zainstalowanego pliku zgodne z oryginałem |
| Kontrola pobranych plików poza CI | PASS — wszystkie SHA256, CRC ZIP, struktura bundla, AMD64 PE32+ DLL, eksporty InitDll/GetPluginFactory/ExitDll, tożsamość commitu, źródła i brak historii Git |
| Linux | PASS CTest 2/2, 8 testów infrastruktury, host VST3, GUI i WAV |
| Linux GUI 200% | PASS 2640×1600, kabel aktualizuje DSP i klawiatura generuje dźwięk |

Pełne świeże [logi Windows](evidence/Jerzy-2026-11-01/windows/), [kontrola plików](evidence/Jerzy-2026-11-01/windows/Jerzy-verification.json), [sumy](evidence/Jerzy-2026-11-01/windows/evidence-SHA256SUMS.txt) i [screenshoty](screenshots/). Powtarzalna kontrola pobranych plików: `python scripts/verify-delivery.py` z repo, pliki i status w `out/delivery`.

Pełne zrzuty panelu Windows pochodzą z rzeczywistego komponentu edytora. Dodatkowy screenshot `Jerzy-screenshot-Jerzy-loaded-vst3.png` pochodzi z natywnego okna załadowanej wtyczki; pulpit runnera ma tylko 1024 piksele szerokości, więc prawa część tego konkretnego zrzutu jest czarna poza obszarem pulpitu. Nie traktujemy go jako dowodu pełnego panelu ani testu DPI Windows. Pełne obrazy clean/patched/generative/expert mają 1320×800. Makieta koncepcyjna użytkownika nadal nie jest dostępna.

## Ograniczenia

- FL Studio: skanowanie, Piano Roll, zachowanie okien i szybki bounce **NOT TESTED**. Zaliczone są testy hosta JUCE i pluginval; nie zastępujemy nimi testu FL Studio.
- Instalator nie ma podpisu Authenticode (tablica certyfikatów PE jest pusta).
- Minimalny logiczny panel 1320×800, skalowanie do 200%; brak adaptacji do mniejszych ekranów.
- Nadal brakuje pełnych niezależnych głosów bass/ambient/rytm, stylów i pełnych algorytmów Krell/Markov/Euclidean/random walk oraz rozwoju fraz; pełnego S&H/slew, AM/PM/ring modulation, kolorów szumu i pomiarów aliasingu; pełnego EXPERT, MIDI Learn, dostępnej z klawiatury edycji kabli, glide/pressure UI i pomiarów CPU w hoście.
- Brak MPE i zewnętrznego MIDI OUT. Automatyzacja jest blokowa; seek resetuje DSP zamiast rekonstruować poprzedni ogon. Algorytmiczny pogłos nie jest emulacją konkretnego sprzętu.

## Historia diagnozy

Dawna blokada rozliczeń GitHub nie blokuje obecnego uruchomienia. W tym zadaniu Windows wykrył rzeczywisty błąd narzędzia przechwytującego obraz; [niezaliczony przebieg](evidence/Jerzy-2026-11-01/windows-capture-failure/) zachowano. Następnie testy przeszły, lecz archiwum z historią Git przekraczało ograniczenie kanału dostawy. Jawne usunięcie wyłącznie skopiowanej `.git` w generowanym katalogu źródeł rozwiązało problem. Nie usuwano historii projektu ani zależności. Poprzednie logi nie są liczone jako zaliczenie aktualnego kodu.

Konfiguracja instalacji/startu środowiska chmurowego jest zapisana jako draft. Publikacja i odtworzenie w nowym zadaniu pozostają działaniami produktu/użytkownika; nie zostały tu wykonane.
