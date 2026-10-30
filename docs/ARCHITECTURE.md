# Architektura i kontrakty — 0.1.0

`src/Engine.h`: samodzielny monofoniczny silnik C++20; stałe tablice głosów/FX. `src/Patch.h`: typowane porty, stały limit 64 kabli, topologiczne sortowanie, sumowanie ograniczone do ±2. Brak normalizacji ukrytej poza grafem: domyślny patch jest jawną listą kabli; pusty graf oznacza ciszę.

Wyjścia audio są bipolarne. Pitch CV ma 60 półtonów na jednostkę wokół MIDI 48; MIDI i generator produkują różnicę względem tej bazy. Pozostałe CV są znormalizowane; wejście gate ogranicza sumę do 0–1. Audio łączy się tylko z audio, CV z CV; gate przyjmuje też jawne źródła zegara/triggera/CV. Wszystkie cykle, także samopętle, są odrzucane. Nie ma ukrytego opóźnionego feedbacku. Muted cable nie wnosi krawędzi ani sygnału.

GUI lub deserializacja kompiluje cały graf poza audio. Producent jest serializowany przez mutex nieużywany przez audio. Trójbuforowy mailbox przenosi stały graf na granicy callbacku; audio nie zwalnia shared_ptr, nie czeka i nie alokuje grafu. Szybkie kolejne edycje mogą pominąć pośredni patch, lecz zachowują ostatni kompletny. Wyjście po zmianie grafu przechodzi przez 5 ms rampę od ostatniej próbki; zapobiega ona skokowi wartości, lecz nie jest równoległym crossfade dwóch pełnych silników.

`src/Plugin.cpp`: JUCE VST3/standalone, stereo output, brak wejść audio, MIDI in. Parametry hosta mają stabilne identyfikatory i wersję 1. Odczyt atomowych parametrów raz na blok. Note on/off respektuje offset w próbce; zmiany parametrów są blokowe. SysEx nie jest kopiowany w callbacku. Touch keyboard przekazuje zdarzenia atomowo bez `MidiKeyboardState` z blokadą.

`GARDEN` state version 1 przechowuje parametry i `PATCH/CABLE` z from/to/gain/mute. Cały graf i zakresy parametrów są walidowane przed replaceState. Uszkodzony lub nieobsługiwany stan nie zastępuje bieżącego. Pozycje gniazd są stałym layoutem UI, kabel jest odtwarzany z końców; dowolne pozycjonowanie modułów nie jest wdrożone.

Generator wyznacza krok i prawdopodobieństwo z PPQ, seedu i indeksu kroku, bez zegara ściennego i losowania per callback. HOST wymaga prawdziwego PPQ i play; brak playheada nie implikuje play. FREE wymaga run=true. HYBRID traktuje MIDI jako kontekst harmoniczny. Pętla/seek resetuje oscylatory i rezonatory; nie ma rekonstrukcji historycznego ogona po skoku w środek utworu. Ciągłe rendery od tego samego startu są porównywane bitowo między rozmiarami bufora. To nie dowodzi równoważności rzeczywistego bounce FL Studio.

Oscylator: mieszanie sine/triangle/polyBLEP saw/pulse, fold z czterema podpróbkami i filtrem jednobiegunowym. LPG: wygładzana obwiednia z zależnym cutoffem, osobne VCA/VCF. FX: trzy opóźnione rezonatory z ograniczeniem feedbacku. To autorskie algorytmy inspirowane ideą, nie emulacje obwodów 110/208C ani pomiarowa rekonstrukcja sprężyny. Czterokrotne próbkowanie nie gwarantuje braku aliasingu FM/fold; wymaga analizy widmowej i lepszego filtra decymującego.
