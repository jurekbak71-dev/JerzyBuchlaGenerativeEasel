# Research i zależności — 2026-10-30

Weryfikacja wykonana przez Git HTTPS i oficjalne pliki pobrane z GitHub. Odczyt natywnego Git wymagał uprawnionego wykonania, ponieważ zwykły sandbox nie mógł połączyć się z proxy. Nie wyłączano TLS, podpisów pakietów ani weryfikacji artefaktów.

| Źródło | Faktycznie sprawdzone | Licencja / decyzja |
|---|---|---|
| [JUCE 8.0.12](https://github.com/juce-framework/JUCE/tree/29396c22c93392d6738e021b83196283d6e4d850) | Tag i pełny SHA, LICENSE.md, CMake API, źródła procesora/MIDI/GUI. Lokalnie skompilowano z GCC14/C++20. Nie deklarujemy, że to najnowsza wersja JUCE. | AGPLv3 albo komercyjna. Wybrano darmową ścieżkę AGPLv3 dla nowego kodu, bez zakupu licencji. Dystrybucja obejmuje źródła. |
| [SDK dołączone do przypiętego JUCE](https://github.com/juce-framework/JUCE/blob/29396c22c93392d6738e021b83196283d6e4d850/modules/juce_audio_processors_headless/format_types/VST3_SDK/LICENSE.txt) | Pełny plik licencji MIT z copyright 2025. Bundle skompilowany lokalnie używa właśnie tych nagłówków. | MIT; zachowano LICENSES/VST3.txt. Brak VST2 SDK. |
| [Steinberg VST3 SDK](https://github.com/steinbergmedia/vst3sdk) | Odczyt HEAD `3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96`, LICENSE.txt na master: MIT, copyright 2026. | Nie zastępuje przypiętego SDK JUCE; nie mieszano wersji. |
| [pluginval 1.0.4](https://github.com/Tracktion/pluginval/tree/ed19c2c16b57a6d94db391bea3ef4a80b769d5bf) | Aktualny stabilny tag spośród odczytanych tagów (2.0.0 występuje jako beta), CMake, CLI, submodule JUCE `5179f4e720d8406ebd1b5401c86aea8da6cc83c9`. | Narzędzie testowe, nie linkowane z instrumentem. Własne przypięte dependencies. |
| [Buchla Music Easel](https://buchla.com/product/music-easel-modern/) | Próba pobrania: HTTP 403 w środowisku. Nie przeanalizowano strony ani schematów. | Nazwa i koncepcja historyczna nie są zgodą na kopiowanie grafik/kodu. W UI autorska nazwa Voltage Garden, bez logo Buchla. |
| [FL Studio wrapper manual](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/plugins/wrapper.htm) | Próba pobrania: HTTP 403. Brak uruchomionego FL Studio. | Funkcje hosta są implementowane przez JUCE playhead/MIDI; zgodność z FL pozostaje NOT TESTED. |

Domeny researchu i API GitHub zapisano w szkicu ustawień sieci. Zapis szkicu nie jest zastosowaniem ustawień do aktualnego runtime. API Actions nadal odpowiada `Forbidden`; odczyt Git działa. Brak nowych sekretów nie oznacza braku uwierzytelnienia GitHub — istniejący proxy pozwolił odczytać wszystkie repozytoria referencyjne.

## Rozróżnienia techniczne

Historyczny Buchla 110 był sekcją gate/VCA; późniejsze LPG o charakterystyce optoelektronicznej łączą zmianę amplitudy z barwą i zależnym od częstotliwości wybrzmieniem. W tym projekcie nie ma pomiarów elementów ani netlisty analogowej: LPG jest matematyczną obwiednią i filtrem, tryby VCA/VCF mają odrębną semantykę. Nie twierdzimy, że odtworzono schemat 110 czy 208C. To założenie projektowe wymagające potwierdzenia w materiałach źródłowych po odblokowaniu dostępu, a nie wynik analizy niedostępnego PDF.

Saw/pulse stosują polyBLEP, wavefold cztery podpróbki. Brak opublikowanej analizy widmowej nieliniowego FM oznacza brak dowodu spełnienia docelowej kontroli aliasingu. Reverb to trzy algorytmiczne rezonatory, nie model modalny fizycznej sprężyny. Generator ma powtarzany motyw, kwantyzację skali, progresję stopni, gęstość, swing, zmienny gate i dynamikę; nie ma jeszcze kompletnego harmonic brain, Markova, pełnego Krella ani osobnych głosów bass/ambient.

Nie kopiowano z forów ani nie przypisywano im autorytetu bez sprawdzenia. Aktualny research Buchla, Image-Line, publikacji DSP oraz porównanie makiety pozostają otwarte.
