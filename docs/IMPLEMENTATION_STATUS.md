# Jerzy Voltage Garden 0.2.0 — zweryfikowana dostawa

Data: 2026-11-01. GUI przebudowano według zdjęcia wskazanego i zatwierdzonego przez użytkownika jako docelowe. Gotowe pliki zostały pobrane, sprawdzone niezależnie od CI i udostępnione poniżej. **Nadal jest to wersja rozwojowa silnika, nie pełna realizacja MASTER_PROMPT.**

## Pobieranie

- [Windows x64 VST3 ZIP — 4 176 275 bajtów](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/JerzyVoltageGarden-0.2.0-Windows-x64.zip)
- [Instalator Windows — 5 542 662 bajty](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/JerzyVoltageGarden-0.2.0-Windows-x64-Setup.exe)
- [Pełne źródła z JUCE — 28 689 046 bajtów](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/JerzyVoltageGarden-0.2.0-Source.zip)
- [Rzeczywiste GUI wtyczki załadowanej na Windows](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/raw/335319f8ca943df895ef72f54c5f66be75ea1a7e/Jerzy-screenshot-Jerzy-loaded-vst3.png)
- [Artefakt Actions](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/actions/runs/41327400349/artifacts/13091679335)

Bezpośrednie linki przetestowano przez HTTP 206 oraz sygnatury ZIP/EXE. Są przypięte do commitu dostawy `335319f8ca943df895ef72f54c5f66be75ea1a7e`. Kod kompilacji: `57b502dd874e1098c894ac97f6ea7a45a388835a`. [Uruchomienie Windows](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/actions/runs/41327400349). Późniejsze zmiany raportów nie oznaczają nowej kompilacji aplikacji.

## Nowy układ i proporcje

Szeroka jasna górna płyta zajmuje około połowy korpusu. Ma jeden bank **20 długich suwaków**, lewą sekcję **12 precyzyjnych gałek i wyświetlacza**, prawą kolumnę niebieskich gałek i czerwony PANIC. Na dole są ciemna płyta marki Jerzy oraz niebiesko-czerwony **29-kontaktowy kontroler** o szerokości około 81% górnego panelu. Domyślny zakres klawiatury: C3–E5, ze zmianą oktawy. Nie kopiowano fotografii jako tła ani logo Buchla.

Suwaki i gałki korzystają z istniejących parametrów hosta; nie są dekoracją. Zachowano identyfikatory parametrów, format stanu, presety i rzeczywisty typowany graf kabli. EXPERT pokazuje diagnostykę na lewej płycie. Logiczny rozmiar: 1320×920; proporcja jest zachowywana w zakresie 924×644 do 2640×1840 (70–200%). [Porównanie kompozycji](GUI_REFERENCE_QA.md).

## Rzeczywiste sprawdzenia

| Sprawdzenie | Wynik |
|---|---|
| Windows x64 MSVC / JUCE 8.0.12 | PASS — VST3, standalone, testy |
| Windows CTest | PASS 2/2, 45 sprawdzeń rdzenia |
| pluginval 1.0.4 strictness 5 | PASS — faktyczny bundle 0.2 |
| Host JUCE ładuje gotowy VST3 | PASS — skan, instancja, cisza bez MIDI, audio po Note On, zapis stanu, natywny edytor i zamknięcie |
| Przebudowane GUI Windows | PASS — clean/patched/generative/expert; drag kabla zmienia graf DSP |
| Długie suwaki | PASS — zmiana TIMBRE trafia do DSP, gałka SHAPE synchronizuje tę samą wartość |
| Klawiatura dotykowa | PASS — dźwięk, przesuwanie, zwolnienie poza panelem oraz skrajny 29. kontakt E5 |
| Offline WAV | PASS — 48 kHz/24 bit stereo, odczyt próbek; równoważność renderu dla bloków 32 i 1024 |
| ZIP / instalator / uninstaller | PASS — wykonane na Windows; zainstalowany plik ma SHA256 oryginału |
| Pobrane pliki poza CI | PASS — wszystkie SHA256 i CRC ZIP, AMD64/PE32+/DLL, rzeczywista tablica eksportów z GetPluginFactory, tożsamość commitu i źródła bez historii Git |
| Linux | PASS CTest 2/2, 8 testów infrastruktury, GUI, host, WAV |
| Skalowanie Linux | PASS — 70%, 100%, 200%; wymiary, rzeczywisty routing i dźwięk klawiatury |
| Dopasowanie okna Windows | PASS — natywny screenshot obejmuje pełny panel na małym pulpicie runnera, bez czarnego fragmentu poza ekranem |

[Logi](evidence/Jerzy-0.2/), [kontrola plików](evidence/Jerzy-0.2/windows/Jerzy-verification.json), [sumy](evidence/Jerzy-0.2/windows/evidence-SHA256SUMS.txt), [screenshoty](screenshots/0.2/). Powtórzenie kontroli: `python scripts/verify-delivery.py out/delivery-0.2` z repo.

Zrzuty pochodzą z działającego kodu JUCE, a dodatkowy obraz natywnego okna z faktycznie załadowanej wtyczki Windows. Porównano widoczną fotografię użytkownika z tymi obrazami. Przy małej rozdzielczości źródła nie deklarujemy odczytu mikrotekstu ani zgodności pikselowej; odwzorowano kompozycję, proporcje i charakter sprzętu z autorskimi oznaczeniami Jerzy.

## Pozostałe ograniczenia

- FL Studio: skanowanie, Piano Roll, zachowanie okien i szybki bounce **NOT TESTED**. Host JUCE i pluginval nie są testami FL Studio.
- Instalator niepodpisany Authenticode.
- Pełne niezależne głosy bass/ambient/rytm, style i pełne algorytmy Krell/Markov/Euclidean/random walk, rozbudowany S&H/slew, AM/PM/ring modulation i kolory szumu pozostają poza obecnym silnikiem.
- Pozostają pełny EXPERT/routing inspector, MIDI Learn, edycja kabli klawiaturą, pressure/glide UI, szczegółowe pomiary aliasingu i CPU w hoście. Brak MPE i zewnętrznego MIDI OUT. Automatyzacja jest blokowa, seek resetuje DSP zamiast rekonstruować ogon.
- Rozmiar 70% pozwala zmieścić cały panel, lecz do odczytu drobnych etykiet zalecany jest większy widok. Nie jest to emulacja konkretnego sprzętu.

## Poprzednia wersja i środowisko

Zweryfikowane pliki 0.1.0 pozostają dostępne pod historycznymi linkami z [raportu 0.1](https://github.com/jurekbak71-dev/JerzyBuchlaGenerativeEasel/blob/a5b76a2279740b28cdcc2d0d07d85f95d2924b6c/docs/IMPLEMENTATION_STATUS.md). To inny, starszy układ GUI. Nie używamy ich jako dowodu dla wersji 0.2.

Konfiguracja instalacji i uruchamiania środowiska jest zapisana jako draft. Publikacja i odtworzenie w nowym zadaniu nie były tu wykonywane.
