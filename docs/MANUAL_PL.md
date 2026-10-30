# Voltage Garden 0.1.0 — instrukcja robocza

To wersja rozwojowa. Status sprawdzonych platform i brakujących funkcji jest w IMPLEMENTATION_STATUS.md. Nie jest to autoryzowany produkt Buchla ani wierna emulacja sprzętu.

1. W trybie MIDI zagraj nutę z kontrolera/Piano Roll lub dolnej klawiatury. Domyślnie generator się nie włącza. Note Off i pedał sustain kontrolują bramkę; pitch bend ma zakres ±2 półtony, CC1 zmienia głębokość FM po podłączeniu źródła do FM.
2. GENERATIVE z HOST wymaga uruchomionego transportu hosta z dostępnym PPQ. W standalone wyłącz HOST i naciśnij FREE START. Sam wybór FREE nie startuje dźwięku. HYBRID używa ostatniej nuty MIDI jako kontekstu dla frazy generatora. MANUAL korzysta z tej samej monofonicznej klawiatury co MIDI.
3. Przeciągnij gniazdo OUT do zgodnego IN. Audio jest pomarańczowe, CV niebieskie, gate czerwone, zegar fioletowy; etykiety IN/OUT opisują kierunek. Niezgodne typy i pętle są odrzucane. Wyjście może zasilać wiele wejść, wejście sumuje źródła do limitu ±2. Pitch CV: 1 jednostka = 60 półtonów.
4. Kliknij kabel prawym przyciskiem: mute/unmute, remove, odwrócenie polaryzacji, gain 0.5. NORMAL przywraca jawny patch fabryczny; CLEAR usuwa wszystkie kable i powoduje ciszę. Samo dodanie wyjścia oscylatora do LPG nie da dźwięku bez bramki i drogi do wyjścia.
5. Aby usłyszeć modulację: OUT MOD do IN FM; zwiększ FM DEPTH. Sekwencer ma pięć regulowanych wartości w półtonach; jego OUT SEQ 5 podłącz do PITCH CV. Istniejące źródło pitch i sekwencer sumują się. RANDOM do FOLD CV zmienia barwę krokowo. PULSER może bramkować LPG.
6. LPG I i II są oddzielnymi torami; wspólne DECAY/CUTOFF i wybór LPG/VCA/VCF. Oscylator ma ciągłe SHAPE oraz FOLD. SPRING WET/DRY miesza sygnał z algorytmicznym rezonansem, nie próbką sprzętu.
7. SAVE/LOAD zapisuje plik `.vgarden`: parametry, seed, tryb, routing, mute i gain. Host zapisuje ten sam stan w sesji. Nie zapisujemy pozycji kursora renderu ani ogona reverb; otwarcie sesji i render od tego samego startu jest odtwarzalne. PANIC przełącza na MIDI i wyłącza lokalne run, zwalnia nuty; ogon efektu może jeszcze wybrzmieć.
8. EXPERT pokazuje limit kabli, skalowanie i peak; nie jest jeszcze kompletnym edytorem routingu. Rozmiar panelu można powiększać do 200%. Brak pełnej obsługi kabli z klawiatury jest znanym ograniczeniem dostępności.

Docelowy Windows: folder `Voltage Garden.vst3` musi trafić w całości do `C:\Program Files\Common Files\VST3`. Nie kopiuj jedynie wewnętrznego pliku. Instalator wymaga uprawnień administratora. Nie potwierdzono jeszcze budowy/instalacji Windows ani skanowania przez FL Studio; instrukcja opisuje zamierzony układ paczki, a nie ukończony test hostowy.
