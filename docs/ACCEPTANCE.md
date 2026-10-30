# Definition of done / testy odbioru

Nie uznawaj projektu za ukończony bez weryfikowalnych dowodów.

## Kompilacja i dostawa
- VST3 Windows x64 w prawidłowej strukturze bundla i ładujący się w hoście.
- Wersjonowany ZIP, instalator Windows, instrukcja PL i release notes.
- CI Windows MSVC z linkiem do pobrania artefaktów, hashe SHA-256.
- Wersja CMake, JUCE, VST SDK i licencje przypięte/powtarzalne.

## Testy funkcjonalne
- Note On/Off, velocity, pitch bend, modwheel, sustain, aftertouch (zgodnie ze wsparciem hosta).
- FL Studio MIDI Piano Roll i tryb bez MIDI.
- Host BPM, changes, start-stop, host loop, timeline discontinuity, beat boundaries; free-run.
- Sekwencer/generator nie startuje w MIDI mode bez wyraźnego włączenia.
- Przełączanie patch cords wpływa na dźwięk; typowane kable; zapis i restore.
- Presety, host state, root/scale/seed, sequencer and all routing restore after restart.
- Bounce WAV w offline/faster-than-realtime: brak ciszy, brak resetowania do default i równoważność generative stream.
- Powtarzalny seed/transport daje powtarzalny wynik; stop nie zostawia hung notes.
- Zero NaN/Inf, dzikich feedback spikes, blocking calls w audio thread.
- Low CPU; pamięć bez wycieków; testy 44.1/48/96k i buffer 32/64/256/1024.
- Stres, automatyzacja, otwieranie/zamykanie GUI, powtórne ładowanie wtyczki.
- Screenshoty rzeczywistego GUI, porównanie z zatwierdzonym kierunkiem.

## Jak raportować
W raportach rozdziel PASS / FAIL / NOT TESTED; dodaj polecenie/test host, build hash i link do artefaktu. Jeśli brak licencji, zależności lub dostępu do Windows / FL Studio, przyznaj to wprost.
