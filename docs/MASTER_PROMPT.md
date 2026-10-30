# MASTER PROMPT — Jerzy Buchla Generative Easel
**Wykonuj, nie tylko planuj.** Data założeń: 2026-10-30. Język instrukcji i raportów: polski.

## 0. Cel i kontekst
Utwórz od podstaw autorski, złożony instrument VST3 Windows x64 do FL Studio, inspirowany Buchla Music Easel (208C/218e/EMBIO), historycznymi Buchla 100/200 i współczesną muzyką generatywną. Użytkownik nie programuje: minimalizuj czynności ręczne. To ma być gotowy, działający produkt, a nie demonstracja. Nazwa `Jerzy Buchla Generative Easel` jest robocza — użyj autorskiej nazwy w UI i nie sugeruj autoryzacji producenta.

Read AGENTS.md, docs/GUI_SPEC.md, docs/EXISTING_REPOS.md, docs/ACCEPTANCE.md, docs/SOURCES.md, references/README.md. Nie zakładaj istnienia załączników z rozmowy, jeśli nie są w repo. Przeprowadź dostępny research online z dokumentacją źródeł i licencji.

## 1. Wykorzystanie dostępnych umiejętności i repozytoriów
Sprawdź dostępne narzędzia, Skills, aplikacje i integracje, a następnie użyj adekwatnych: GitHub, CI, dozwolone repozytoria, dokumentacja, testy. Nie zakładaj dostępności bez sprawdzenia. Przejrzyj podane w docs/EXISTING_REPOS.md rzeczywiste repozytoria użytkownika — tylko read-only. Przejmuj wyłącznie sprawdzone, legalnie ponownie używalne moduły; inaczej implementuj samodzielnie. Raport reuse audit. Nie zmieniaj cudzych projektów.

## 2. Technologie i struktura
Wybierz C++20/JUCE/CMake/MSVC; przypnij wersje i zapisz licencje. Oddziel DSP, scheduling, transport, host bridge, serializację, renderer UI, testy. Utwórz repozytorialny kod źródłowy, CMakePresets, rozwiązanie CI Windows, testy unit/integration, presets, docs i packaging. Bez płatnych zależności wymaganych do buildowania, chyba że są niezbędne i użytkownik jawnie wyrazi zgodę.

## 3. Rdzeń DSP
- Complex Oscillator: precyzyjna regulacja pitch, dynamic wave shaping, triangle/sine/saw/pulse, wavefolding, timbre, FM/AM/PM, detune, sync jeśli technicznie uzasadniony.
- Modulation Oscillator: LFO/audio voice, modulations cross-route, S&H, slew, noise.
- Dual LPG: osobno VCA/VCF/LPG, zależne częstotliwościowo wygaszanie, różne obwiednie, stabilność filtrów. Rozróżnij historyczne modele Buchla 110 i nowsze vactrol LPG.
- Historical tools: ring modulation, optional band-pass, noise colors, pulse generator, envelope generator, 5-stage sequencer, random voltage, CV processors, crossfader/attenuverter.
- FX: spring-like algorithmic reverb, saturation, stereo, optionally analog/tape/delay techniques from user libraries after quality review.
- Anti-aliasing dla waveform/wavefold/filter/saturation; dynamic oversampling jeśli sensowny; brak aliasingu w podstawowych testach.
- Sound engine ma produkować sensowny dźwięk bez zewnętrznego MIDI w trybie generatywnym.

## 4. PATCHING WIRTUALNYMI KABELKAMI — NIEPODLEGAJĄCE NEGOCJACJI
Są to **prawdziwe wirtualne kable na panelu** łączące gniazda audio, CV, gate, trigger, clock, podobne w ergonomii do banana patch cords.
- Drag/drop z output do input; bezpośredni widoczny kabel, z krzywizną i animacją podłączenia.
- Patch graph jest jedynym źródłem prawdy DSP, nie sztucznym overlay ani samą matrix modulation.
- Typy portów, zakresy napięć normalizowanych, polarity/scaling i bezpieczna konwersja.
- One-to-many outputs, multiple-input sum with bounded gain, inspect/mute/remove/replace wires.
- Obsługa kabli od random, pulser, 5-step seq, kre ll/generative, MIDI-derived CV, OSC, LPG, FM, FX.
- Graph changes poza callbackiem; atomic/safe swap, smoothing, bez malloc/locks/glitch w audio.
- No illegal zero-delay feedback: reject loop or insert explicit stable unit-delay and feedback limiter.
- Cable positions, parameters, routing zapisz w presetach i FL Studio project state.
- Test integracyjny musi udowodnić, że dodanie kabla zmienia sygnał audio albo modulowany parametr.

## 5. GENERATIVE MUSIC ENGINE — autonomiczny i muzykalny
Nie jest losowym generatorem nut ani banalnym arpeggiatorem.
- Tryby MANUAL / MIDI / GENERATIVE / HYBRID.
- W GENERATIVE sam produkuje frazy, bas, melody/ambient/drone, rytmiczne impulsy i modulations, bez MIDI input.
- Harmonic brain: root, scale, chord progression, harmonic rhythm, voice leading, tension release, motif memory, phrase development, note duration variation, gate, articulations, swing/dynamics, probability density.
- Generative algorithms: Krell, controlled random walk, S&H, Markov transitions, Euclidean/polyrhythmic sequencing, controlled mutations, bounded stochastic fields, independent cycles.
- Musical groove first: quantization with humanization; avoid musically incoherent phrases, poor chord voicing, excessive glitch, stuck long notes. Styles: ambient, dark ambient, drone, Berlin school, minimal, experimental, IDM, techno, acid.
- Real separate root/motion response; musical labels rather than opaque parameter names. Controlled lofi glitch, not random disruptive noise.
- Deterministic seed + transport-based scheduling for offline/export and repeatable reload.
- Host sync and self-running free clock (local tempo). START/STOP independent but clear: HOST mode defaults follow transport; FREE mode can run without transport only on explicit user start. All notes off on stop, tails controlled separately.
- HYBRID listens to MIDI harmonies; MIDI mode must NEVER start generator uninvited.
- All generators can be sources in patch graph.

## 6. FL Studio, MIDI, host
- Correct VST3 instrument bus/layout, stereo, host automation, MIDI NoteOn/Off, velocity, sustain, pitch bend, mod wheel/CC, channel pressure, poly aftertouch where support exists; optional MPE after verified capabilities.
- Piano Roll playback and note gate; MIDI Learn where appropriate.
- Host BPM, meter, PPQ/beat position, play/stop, tempo changes, loop, time jumps and sample-accurate event scheduling as feasible.
- Clock divisions 1/1–1/128 with dotted/triplet; sequence, LFO, delay and generative phrase sync.
- Full state save/load (including synth, patch cords, sequencing, seed, mode, GUI layout) and stable preset recall. No reset to initial sound on reopened project.
- Offline/export WAV: same mode/patch/sounds/notes, no default settings restore, no silent export, no accidental sequencing instead of Piano Roll. Deterministic regeneration based on state and musical timeline. Explicitly test offline (fast-than-real-time) rendering.
- Distinguish plugin's internal generator events from MIDI OUT. External MIDI OUT only if realistically supported by host, with independent documented tests.

## 7. GUI — kierunek zatwierdzony
Wizualizacja koncepcyjna z rozmowy zaakceptowana: metaliczny, jasny hardware-looking instrument Buchla-style with orange/blue/red jacks, physical sliders and knobs, panels, bottom touch keyboard, top BPM/sync/preset bar. Nie stosuj oficjalnego logo.
PANEL: OSC, modulation, LPG, S&H/random, sequencer, pulser, generative, mixer, FX, CV/patch cables. EXPERT: full routes, graph diagnostics, precision params. Controls visual + keyboard and patch connections must function.
Obowiązkowe real-screen screenshots z już uruchomionego GUI: panel clean, patched, generative, expert. Porównaj z referencją; gdy brak autentycznego obrazu zaakceptowanej makiety, wskaż brak zamiast udawać zgodność. User zatwierdził kierunek GUI — nie wymagaj ponownego zatwierdzenia tych samych założeń; istotne zmiany wizualne pokaż przed przyjęciem.

## 8. Produkcyjne bezpieczeństwo DSP
Realtime constraints: never allocate, lock mutex, perform file/network/GUI I/O in processBlock. Separation of host processing and UI, denormal handling, NaN/Inf sanitization, sample rate changes, voice limit, parameter smoothing, feedback limiter, test worst-case routing, CPU profiling. GUI must not produce blank/black windows in FL Studio. Stable lifetime and reset.

## 9. Audyt, testy i rzeczywiste dowody
Automated: CTest or equivalent, DSP pitch, OSC/FM/Wavefolder, LPG decay and frequency, event scheduler, transport and loop, graph safety, serialisation roundtrip, deterministic generative, offline equivalence, plugin validator, leak/stress at various sample rates and block sizes. Host actual FL Studio test, if accessible; if not, mark NOT TESTED instead of PASS.
CI: Windows-latest + MSVC, release build with VST3 bundle, tests, artifacts, package with release notes / hashes. If hosting unavailable, report exact blocker and best deliverable.
Installer: Inno Setup or comparable free tool, with default VST3 installation directory; also ZIP portable installation package. Use Windows VST3 structure. Include PL docs and presets.
Log errors and repair, re-run until green or real external blocker.

## 10. Etapy
A. Research + asset check + reference repo audit + legal/architecture decisions.
B. Minimal sound-producing VST3 test + working build and CI.
C. Real patch graph and DSP modules with tests.
D. Stable autonomous generative and host integration with offline tests.
E. Approved-direction GUI + actual screenshots + functional patch wires.
F. Integration + FL Studio verification where possible, CPU stability.
G. Release Windows x64 VST3 + installer + ZIP + PL manual + actual download links.

Wykonuj autonomicznie wszystkie etapy bez zbędnych pytań. Jedyny obligatoryjny punkt konsultacji: istotna zmiana względem zatwierdzonego wyglądu lub potrzeba wyraźnej zgody (np. licencja płatna, działania poza repo). Nie deklaruj gotowości przed zrobieniem finalnej kompilacji/testów i publikacją artefaktów.

## 11. Output
Na koniec zwróć URL do commit/release i CI artifacts: .vst3, .exe, .zip, screenshots, test report, hashes. Jeśli brak FL Studio w środowisku, zaznacz to. Aktualizuj status w repo i raportuj wyłącznie wykonane rzeczy. Kod musi być dostępny i możliwy do ponownej kompilacji.

**Nie kończ na promcie, mockupie, prototypie lub tekstowym podsumowaniu. Kończ działającą, rzeczywiście zweryfikowaną wtyczką VST3 Windows x64.**
