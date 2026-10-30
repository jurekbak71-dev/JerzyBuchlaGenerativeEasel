# GUI — zatwierdzony kierunek i wymagania

Stan 2026-10-30: Użytkownik zaakceptował **kierunek wizualny** pokazanej makiety: jasny metaliczny panel, stylistyka vintage Buchla, kolorowe banana-jacks, suwaki, gałki, sekcje syntezy, podłużna klawiatura touch oraz pasek BPM/transport. Użytkownik dodatkowo wymaga **fizycznie widocznych wirtualnych kabli**.

> Wygenerowany w rozmowie obraz koncepcyjny nie jest rzeczywistym screenshotem działającej aplikacji i nie został jeszcze umieszczony w repo. Zob. references/README.md.

## Layout
Górny pasek: nazwa (autorska, bez oficjalnego logo Buchla), preset, save/load, transport, BPM, HOST/FREE, MIDI indicators, CPU, panic.

Panel główny: Complex OSC, Mod OSC, Wavefolder, Dual LPG, mod source, noise, ring modulation, envelope, pulser, random voltage, 5-step seq, mixer, spring reverb, patch gniazda.

Warstwa generatywna: scale/root, mood, density, phrase length, harmony, bass, ambient, rhythm, variation, kre ll/random (etykieta 'Krell'), probability, seed, deterministic restart, independ. start/stop.

Dolna część: klawiatura dotykowa, oktawy, pressure, glide, preset voltages, aftertouch, MIDI learn.

Widoki PANEL i EXPERT. EXPERT daje wizualizację połączeń, diagnostykę, patch inspector i precyzyjny routing bez ukrywania kabli na PANEL.

## Virtual patch cords — krytyczne
- Drag from output jack to compatible input jack. Curve kabel, naturalny zwis i śledzenie podczas przeciągania.
- Rozróżnienie audio/CV/gate/trigger/clock kolorami, ale również tekstem/ikoną (dostępność).
- Rzeczywisty graf sygnału DSP/CV; przełączanie w safe boundary bez glitchów i bez locków audio.
- Fan-out wyjść i wielokrotne źródła wejścia tylko z jasno określoną semantyką miksowania/sumowania.
- Delete/replace/mute/inspect connections, cable visibility and per-cable gain/polarity where sensible.
- Save/restore complete patch in preset and DAW session.
- Feedback cycles: detect; reject unbounded zero-delay loop or enforce explicit sample delay and bounded feedback.
- Typowane gniazda, zakresy CV, conversion scaling, smoothing and protection; single source of truth DSP graph.

## Kryterium wizualnej akceptacji
Wygeneruj z **prawdziwego GUI** co najmniej screenshots: panel bez kabli, panel z połączeniami, GENERATIVE, EXPERT. Zapisz je jako artefakty. Zestaw screenshoty z referencją użytkownika; brak obrazka w repo oznacza konieczność jego pozyskania, a nie zezwolenie na dowolne GUI.

Skalowanie 100–200%, High DPI, font legibility, dynamic redraw efficiency, FL Studio window behavior, no black screens, no sluggish rendering.
