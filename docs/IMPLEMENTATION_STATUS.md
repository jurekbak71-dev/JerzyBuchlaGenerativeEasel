# Aktualizacja 2026-11-01 — Jerzy Voltage Garden

Trwają testy bieżących zmian: prefiks Jerzy w produkcie i paczkach, kontrast GUI, oktawy, przesuwanie nut dotykowej klawiatury i zwalnianie poza panelem, widoczność kabli bez ingerencji w DSP. Dodano test ładowania rzeczywistego bundla VST3, renderu MIDI, stanu i otwarcia edytora. ZIP binarny oraz źródła są rozdzielone. Dostawa plików przez ograniczony kanał Git raportów eliminuje zależność pobierania od API Actions.

PASS obecnego przebiegu: rdzeń CTest Linux; 8 testów infrastruktury, w tym binarny roundtrip ZIP przez Git. Kompilacja i dalsze testy pozostają w toku; ten wpis nie stanowi potwierdzenia gotowości Windows.

Nowszy od poprzedniego opisu raport `4f265a078a91764911a31df003f5a9fdba279cc1` potwierdza dla starszego kodu `b0a23ddc44395b334012133da94ff31404b4bf9d` sukces pluginval oraz instalacji i odinstalowania. Nie jest dowodem dla bieżących zmian. API Actions zwraca Forbidden, a próba pobrania poprzedniego artefaktu 404.

Lokalny MinGW nie jest obsługiwany przez JUCE 8; nie zmieniano upstream, żeby omijać tę kontrolę. Alternatywne pobranie Microsoft SDK przez aka.ms odrzucone przez proxy (403). Kompilacja Windows pozostaje w istniejącym workflow MSVC.

Brak oryginalnej zatwierdzonej makiety w references; brak FL Studio. Pozostałe ograniczenia funkcjonalne opisane poniżej nadal obowiązują.

---

# Rzeczywisty stan — 2026-10-31

**Projekt pozostaje wersją rozwojową, nie spełnia pełnego MASTER_PROMPT. Nie ma jeszcze potwierdzonego instalatora ani ZIP Windows do pobrania.**

**Potwierdzona blokada zewnętrzna: rozliczenia konta GitHub.** Właściciel odczytał adnotację dla uruchomienia `41187716045`, job `124497917710`: "The job was not started because recent account payments have failed or your spending limit needs to be increased. Please check the 'Billing & plans' section in your settings". Nie jest to wynik kompilacji. Nie zwiększano limitów, nie inicjowano płatności ani nie zmieniano widoczności repozytorium. Dalsze próby CI wstrzymano do rozwiązania blokady przez właściciela konta.

## Co rzeczywiście sprawdzono

| Obszar | Wynik i źródło dowodu |
|---|---|
| Windows x64 / MSVC 19.51 | PASS kompilacji VST3 i standalone dla `6bc9c361dfa1af6bfcb3174aa9f22d3b0ba0e5e5`; [log](evidence/windows-msvc-44/evidence-build.log) |
| Windows CTest | PASS 2/2, 44 sprawdzenia rdzenia; [pełny LastTest](evidence/windows-msvc-44/evidence-LastTest.log) |
| Windows GUI i patch cords | PASS: cztery screenshoty prawdziwego edytora, odmienny EXPERT oraz drag aktualizujący DSP; [log](evidence/windows-msvc-44/evidence-gui.log) |
| Windows offline WAV | PASS: stereo 48 kHz/24 bit, zapis i odczyt kontrolny próbek; [log](evidence/windows-msvc-44/evidence-audio.log). To nie jest test FL Studio. |
| Windows pluginval | NOT RUN: run zatrzymała kompilacja samego pluginval (C3688, brak `<chrono>`), a nie wynik walidacji wtyczki; [log](evidence/windows-msvc-44/evidence-validator-build.log) |
| Windows instalator i ZIP | NOT RUN: etapy po nieudanej kompilacji pluginval zostały pominięte |
| Aktualny rdzeń Linux | PASS: 45 sprawdzeń, w tym utrzymanie pitch po Note Off; ASan/UBSan/LSan PASS; [sanitizery](evidence/asan-release-pitch.txt), [CTest](evidence/ctest-release-pitch.txt) |
| Linux GUI i pluginval 5 | PASS po ostatniej poprawce MIDI. Osobny Steinberg validator pozostaje SKIPPED, nie jest zaliczany jako PASS. |
| GUI 200% Linux | PASS: 2640×1600, kabel trafia w rzeczywisty graf, klawiatura produkuje audio; [test](evidence/gui-scale-200.txt), [obraz](screenshots/panel-200.png), kod `tests/GuiScaleProbe.cpp` |
| Workflow | PASS actionlint 1.7.7; [proweniencja](evidence/actionlint.json). To walidacja konfiguracji, nie dowód uruchomienia CI. |
| FL Studio / skanowanie / Piano Roll / szybki bounce | NOT TESTED: brak FL Studio w środowisku |
| Porównanie z zatwierdzoną makietą | NOT TESTED: w repo brak oryginalnego PNG, PDF i fotografii z rozmowy |

## Ostatnie poprawki i uruchomienia

Naprawiono velocity poprzedniej przytrzymanej nuty, powtórzony Note Off pod sustainem, zatrzymanie pulsera z HOST, działanie zegara FREE niezależnie od trybu generatora oraz skok wysokości ogona do domyślnego C po Note Off. Test ostatniego błędu najpierw rzeczywiście FAIL, po poprawce PASS.

Poprawka kompilacji pluginval używa jawnego dołączenia standardowego `<chrono>` przez `/FIchrono` i CMake project hook. Zachowuje domyślne flagi MSVC i wszystkie testy; nie modyfikuje upstream i nie wyłącza walidacji. Cache poprawnie skompilowanych wyników przygotowano, ale po szybkich awariach przed pierwszym krokiem wyłączono go diagnostycznie. Pozostaje pełne budowanie i wszystkie wymagane testy.

Zmiany wysłano jako `f4e0dd9787dc9b50c0b83f6f5e6d8ef5aaa7f6ba`; zdarzenie CI ponowiono przez commit `9834437278e8ac0b2156ab6e2fec54c52b021646`. Dla obu nie otrzymano jeszcze raportu startowego. **Późniejsza adnotacja dostarczona przez właściciela potwierdziła, że job nie wystartował z powodu płatności/limitu konta.** Brak raportu nie jest wynikiem testu.

## Dostęp do CI

API Actions odpowiada `Forbidden`. Po wyraźnej zgodzie użytkownika workflow ma `contents:write` do publikowania ograniczonych raportów na osobnej gałęzi `codex/windows-build-status`. Ten kanał działał i dostarczył powyższe rzeczywiste logi Windows. Nie wymaga proszenia o token w czacie ani zmiany repozytoriów referencyjnych.

Właściciel dostarczył komunikat z [jobu Windows](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/actions/runs/41187716045/job/124497917710), który potwierdza blokadę Billing & plans. Po jej usunięciu należy ponowić aktualny workflow i odebrać raport ze zgodnym SHA. Nie należy ponawiać w kółko zablokowanych uruchomień ani traktować ich jako błędu DSP.

## Zakres i pozostałe wymagania

Działają: rzeczywisty typowany graf audio/CV/gate/clock, fan-out i ograniczone sumowanie, mute/remove/gain/polarity, odrzucanie cykli, mailbox bez blokad/alokacji callbacku; oscylator/fold/FM, mod osc, dwa LPG/VCA/VCF, noise/random/envelope/pulser, pięć regulowanych kroków, mixer i algorytmiczny rezonans; MIDI i podstawowy deterministyczny generator, HOST/FREE, stan parametrów i kabli, presety, działający panel i klawiatura.

Pozostają do realizacji lub weryfikacji: pełne niezależne głosy bass/ambient/rytm, style, Krell/Markov/Euclidean/random walk i rozwój fraz; S&H/slew i pełne niezależne moduły pulse/envelope; AM/PM, ring modulation, kolory szumu; analiza aliasingu i lepsza decymacja; pełny EXPERT, MIDI Learn, dostępna z klawiatury edycja kabli, glide/pressure UI; osobne pole stereo, kontrola tails i pomiary CPU w hoście. Brak MPE oraz zewnętrznego MIDI OUT. Automatyzacja jest blokowa. Seek resetuje DSP zamiast rekonstruować poprzedni ogon. Instalator nie jest podpisany Authenticode.

Nie wykonano merge do main ani Release. Nie ma zweryfikowanego URL artefaktu Windows. Starsza [historia](BUILD_HISTORY.md) dokumentuje kolejne etapy i wcześniejsze, już rozwiązane blokady; nie zastępuje powyższego aktualnego stanu.


## Diagnoza na podstawie obrazu użytkownika

Zrzut listy Actions potwierdził, że próby `f4e0dd9` i `9834437` faktycznie zakończyły się błędem po 3–4 sekundach. Nie były potwierdzoną kolejką. Nie otrzymano raportu startowego ani komunikatu przyczyny. Zewnętrzna akcja cache była jedyną nowo dodaną akcją `uses` od ostatniego runu, który wykonał kompilację i testy. Jej SHA i manifesty zweryfikowano w oficjalnym repozytorium; w ramach izolacji problemu tymczasowo usunięto ją z workflow. Kompilacja, testy, pluginval i weryfikacja pakowania nie są pomijane. Potwierdzenie przyczyny wymaga komunikatu Annotations albo wyniku próby izolującej.


Potwierdzona przyczyna rozliczeniowa oznacza, że wcześniejsza hipoteza o cache nie została potwierdzona. Akcja cache pozostaje obecnie wyłączona diagnostycznie; wszystkie wymagane kompilacje i testy są w workflow. Przed odblokowaniem konta nie deklarujemy weryfikacji poprawki nagłówka pluginval na Windows ani powstania EXE/ZIP.
