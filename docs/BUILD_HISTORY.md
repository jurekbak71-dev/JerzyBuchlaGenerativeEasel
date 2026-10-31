# Rzeczywisty stan — 2026-10-30

**Projekt nie spełnia jeszcze Definition of Done.** Jest skompilowany i przetestowany przyrost Linux VST3/standalone. Nie ma potwierdzonej kompilacji Windows x64, instalatora ani ZIP Windows; samo istnienie workflow nie jest dowodem. Nie ogłaszamy wydania produkcyjnego.

## Wykonane

- Przeczytano AGENTS, MASTER_PROMPT i całą wskazaną dokumentację; pobrano bez zmian 14 repozytoriów referencyjnych. [Reuse audit](REUSE_AUDIT.md), [research i licencje](RESEARCH.md).
- C++20 / JUCE 8.0.12 / CMake 3.31.6. Rzeczywisty bundle Linux x64: `build/linux/VoltageGarden_artefacts/Release/VST3/Voltage Garden.vst3`. Rzeczywisty standalone również skompilowany i uruchomiony na Xvfb/xfwm4. Brak `/dev/snd/seq`: nie testowano fizycznego urządzenia audio/MIDI.
- Oscylator z sine/triangle/polyBLEP saw/pulse, fold, wejściem FM; mod osc; dwa niezależne LPG z VCA/VCF; noise, random, envelope/pulser i pięć regulowanych kroków CV; sumowanie/saturacja i algorytmiczny rezonans reverb.
- Patch graph steruje DSP: typy portów, sumowanie do ±2, fan-out, mute/remove/gain/polarity, odrzucanie cykli i niezgodnych typów. Stałe 64 kable; trójbuforowy mailbox, blokowa zmiana i rampa wyjścia 5 ms bez alokacji/blokady audio.
- Host/MIDI/Manual/Generative/Hybrid, PPQ/BPM, jawny FREE START, seed, skala/root, powtarzany motyw, progresja stopni, density, swing, zmienny gate/dynamika, kontekst MIDI w Hybrid. Generator nie startuje sam w MIDI.
- Próbkowe offsety MIDI, velocity, sustain, bend ±2 st, CC1, channel/poly pressure dla aktywnej nuty. Pełny stan parametrów i kabli z walidacją zakresów oraz trzema rzeczywistymi plikami presetów.
- Działający edytor z klawiaturą, regulatorami, drag/drop kabli i kontekstowym inspect/mute/remove/invert/gain. Cztery obrazy z rzeczywistego GUI: [screenshots](screenshots/). Brak oryginalnego PNG uniemożliwia porównanie z zaakceptowaną makietą.
- Workflow Windows MSVC x64, pluginval, testy, screenshoty, WAV, Inno Setup, ZIP i SHA256. Pakowanie sprawdza strukturę i nagłówek PE x64 oraz dołącza odpowiadające źródła (w tym JUCE). Skrypty Windows i instalator pozostają **NOT TESTED**, dopóki CI naprawdę ich nie wykona.

## Weryfikacja

| Sprawdzenie | Wynik i granice |
|---|---|
| CTest Linux Release | PASS: 2/2 zestawy, rdzeń 40 sprawdzeń i integracja procesora; [raport](evidence/ctest-linux.txt) |
| ASan + UBSan + LeakSanitizer | PASS: 40 sprawdzeń. W sandboxie LSan był blokowany przez ptrace; powtórzono bez wyłączania kontroli w uprawnionym wykonaniu. [raport](evidence/asan-ubsan.txt) |
| Routing | PASS: pętla/niezgodny typ/NaN odrzucone, 100000 publikacji mailboxa, kabel FM zmienia audio, mute odcina tor, GUI drag zmienia faktyczny graf |
| MIDI/transport/offline | PASS w procesorze: cisza bez nut, offset Note On, sustain release, loop usuwa stale notes, identyczny seed daje bitowo równy render dla 32 i 1024 próbek |
| WAV | PASS: rzeczywisty 48 kHz stereo/24 bit, odczyt długości i próbek z tolerancją kwantyzacji. [raport](evidence/audio.txt). To test procesora, nie FL bounce. |
| Stress | PASS: 44.1/48/96 kHz × bufory 32/64/256/1024, fold/FM/FX; brak NaN/Inf/przekroczeń limitu. DSP begin/swap/sample nie alokuje heap w teście. |
| pluginval 1.0.4, strictness 5 | PASS dla Linux VST3, otwierania/zamykania GUI, automatyzacji i pozostałych wykonywanych prób. [log](evidence/pluginval-linux.txt). Dodatkowy Steinberg validator był SKIPPED (brak jego ścieżki); auval nie dotyczy Linuxa. |
| Powtarzalny setup | PASS: wykonano pełny scripts/cloud-install.sh po przygotowaniu środowiska; ponowne configure/build/CTest i build pluginval zakończyły się powodzeniem |
| Windows MSVC / VST3 / EXE / ZIP | NOT TESTED: brak odczytu wyników Actions; Git API Forbidden. Bez dowodu brak deklaracji sukcesu. |
| FL Studio / Piano Roll / szybki bounce / skanowanie | NOT TESTED: środowisko Linux nie zawiera FL Studio. |
| Zgodność obrazu z makietą | NOT TESTED: references/GUI_Approved_Concept.png nie istnieje; również brak PDF i fotografii z rozmowy. |

## Nadal do implementacji i weryfikacji

To nie jest kompletna realizacja pełnego MASTER_PROMPT. Pozostają: osobne aranżowane głosy bas/ambient/rytm i style, pełny Krell/Markov/Euclidean/random walk, rozbudowana pamięć i rozwój frazy/voice leading; S&H/slew i niezależne pulse/envelope jako pełne moduły; AM/PM, ring mod i kolory szumu; modelowanie i pomiarowy audyt aliasingu/filtrów/fold; pełny routing EXPERT, MIDI Learn, edycja kabla z klawiatury, glide/pressure UI, kontrola tails i pomiary CPU w hoście. Brak MPE i zewnętrznego MIDI OUT. Stereo jest dwukanałowym wyjściem tego samego sygnału; nie ma jeszcze niezależnego pola stereo. Parametry są automatyzowane blokowo, nie sample-accurate. Po seek nie rekonstruuje się wcześniejszego ogona FX. Brak podpisu Authenticode oraz rzeczywistego testu instalatora.

## Dostęp i następny krok

Git odczytuje repozytorium przez istniejący HTTPS proxy. Próba API Actions: `Forbidden`; strona WWW Actions: 404 (bez dostępu do prywatnego widoku). Próby oficjalnych stron Buchla/FL: 403. Nie proszono o token i nie wyłączano zabezpieczeń.

Zapisano szkic `install_script`, `start_skill` oraz dodatkowe domeny sieciowe. Aby zastosować te zmiany, należy przejrzeć i zapisać ustawienia środowiska, a następnie opublikować środowisko. Sam zapis szkicu nie publikuje snapshotu ani nie daje dowodu odtworzenia w nowym zadaniu. Po zastosowaniu ustawień trzeba ponowić odczyt Actions i w razie problemu z uprawnieniami ustalić dostęp do CI — nie dublować sekretów bez diagnozy.

Wysłano gałąź `codex/voltage-garden-implementation`; odczyt `git ls-remote` potwierdził zdalny commit [`77f9cb282a24d234970ee5ecce5ae7dd1f46a75c`](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/commit/77f9cb282a24d234970ee5ecce5ae7dd1f46a75c). Ten commit obejmuje przetestowaną implementację i workflow. Po push ponowiono `gh run list --branch codex/voltage-garden-implementation`: odpowiedź `Forbidden`, więc stan uruchomienia i obecność artefaktów pozostają nieznane, a nie FAIL lub PASS kompilacji. Nie wykonano merge do main ani publikacji Release.

 [Actions](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/actions/workflows/windows.yml). Link jest miejscem uruchomień, nie potwierdzeniem artefaktu. Nie ma jeszcze zweryfikowanego URL pobierania Windows ani Release.


## Wznowienie — obsługa MIDI i dostęp do CI

Naprawiono zachowanie velocity przy powrocie do poprzedniej przytrzymanej nuty i ochronę przed wskrzeszeniem nuty przez powtórzony Note Off przy sustainie. Nowe testy regresji przechodzą; rdzeń ma 42 sprawdzenia. Ponowiona kompilacja Linux i CTest: 2/2 PASS. ASan/UBSan/LSan: 42 PASS. Dowody: `evidence/ctest-midi-fix.txt`, `evidence/asan-midi-fix.txt`. Starszy provenance.json nadal opisuje poprzedni przyrost 40-testowy.

Przygotowano lokalnie dodatkowy mechanizm raportowania wyniku Windows poprzez osobną gałąź Git, z pięcioma testami na lokalnych repozytoriach. **Nie wysłano tej zmiany workflow.** Automatyczna kontrola odrzuciła publikację uprawnienia `contents:write` bez wyraźnej zgody użytkownika. Zgoda na tę konkretną możliwość zapisu jest wymagana przed wysłaniem zmiany; sam brak dostępu do API nie upoważnia do podniesienia uprawnień. Pozostałe poprawki MIDI nie wymagają tego uprawnienia.


## Zgoda na raportowanie CI

Użytkownik wyraźnie zatwierdził `contents:write` do raportów Windows. Wysłano commit `47dbc3f` z mechanizmem publikacji raportu przez Git. Pięć testów lokalnych mechanizmu: PASS. Dane Windows pozostają niepotwierdzone do chwili otrzymania raportu ze zgodnym źródłowym SHA; brak raportu nie jest dowodem powodzenia ani niepowodzenia buildu.

## Pulser i zegar — kolejny przyrost

Zatrzymany transport HOST wyłącza również bramkę jawnie podłączonego pulsera. FREE START uruchamia zegar pulsera także w trybie MIDI, bez uruchamiania generatora nut. Rdzeń: 44 sprawdzenia PASS; CTest Linux: 2/2 PASS; ASan/UBSan/LSan: 44 PASS (`evidence/asan-clock-fix.txt`). Obiekty silnika w testach są alokowane przed badaniem realtime na stercie, aby cały zestaw nie zależał od dużego stosu domyślnego w Linux — test callbacku nadal potwierdza brak alokacji DSP.

Workflow uzyskał okresowe, ograniczone raporty postępu przez zatwierdzoną gałąź Git oraz checkpointy po kompilacji i walidacji. Siedem testów infrastruktury raportowania przechodzi lokalnie, w tym propagacja kodu błędu i raport brakującego programu. Nie oznacza to jeszcze PASS trwającego uruchomienia Windows.


## 2026-10-31 — rzeczywiste wyniki Windows i naprawa narzędzia

Uruchomienie `41121061580`, źródła `6bc9c361dfa1af6bfcb3174aa9f22d3b0ba0e5e5`: kompilacja VST3 x64 MSVC 19.51 PASS; CTest 2/2 PASS (44 sprawdzenia rdzenia); prawdziwy GUI/drag kabla PASS; WAV stereo 24-bit i odczyt kontrolny PASS. Dowody są w `evidence/windows-msvc-44/`. Cały run FAIL na kompilacji narzędzia pluginval 1.0.4: C3688 dla literału `100ms`, brak jawnego nagłówka `<chrono>` przy nowym MSVC. Walidacja bundla, instalator i ZIP nie zostały wtedy wykonane.

Poprawka narzędzia używa CMake project hook i `/FIchrono`, zachowuje domyślne flagi MSVC i wszystkie testy; nie zmienia upstream ani nie wyłącza walidacji. Udane kompilacje są teraz zachowywane w cache przypiętym do obrazu runnera i konfiguracji. Narzędzie jest sprawdzane przed kosztownym buildem wtyczki.

Naprawiono ponadto rzeczywisty błąd ogona MIDI Note Off: pitch CV trzyma ostatnią nutę zamiast przechodzić do domyślnego C podczas release. Nowy test początkowo FAIL, po poprawce PASS. Lokalnie 45 sprawdzeń rdzenia i ASan/UBSan/LSan PASS, CTest Linux 2/2 PASS. Źródła nowego przyrostu wymagają ponownej pełnej weryfikacji Windows.
