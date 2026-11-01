# Kontrola GUI według zatwierdzonego zdjęcia — 0.2

Referencja: zdjęcie fizycznego instrumentu przesłane w rozmowie; użytkownik wybrał „Docelowy wygląd GUI — odwzoruj układ i proporcje”. Porównano widoczne zdjęcie z rzeczywistymi screenshotami nowego komponentu JUCE. Nie użyto fotografii jako tekstury ani nie odtworzono znaku Buchla.

| Element kompozycji | Nowy panel |
|---|---|
| Szeroka jasna górna płyta, około połowy wysokości instrumentu | 400 px z około 806 px obu głównych sekcji |
| Jeden zwarty bank pionowych suwaków | 20 suwaków, wszystkie podłączone do parametrów DSP |
| Niewielkie gałki i wyświetlacz po lewej | 12 precyzyjnych gałek, stan trybu/seed/BPM, wspólny wybór LPG |
| Wąska prawa kolumna niebieskich gałek i czerwonego elementu | SPRING, OUTPUT, LPG FREQ oraz PANIC |
| Ciemna dolna sekcja po lewej, klawiatura po prawej | 232 px lewej płyty; kontroler o szerokości 1024 px, około 81% szerokości górnej płyty |
| Niebieski pas połączeń i czerwony panel kontaktowy | Osiem prawdziwych źródeł CV/gate/clock; 29 działających kontaktów C3–E5 |
| Srebrna rama i mechaniczny charakter | Obramowanie, wkręty, kreski skali, metaliczne suwaki i kontakty |

Proporcje zdjęcia są ocenione orientacyjnie; źródło ma małą rozdzielczość. Nie deklarujemy dokładnego odczytu mikrotekstu ani zgodności pikselowej z fotografią. Nazwy i liczba kontrolek odpowiadają rzeczywistym możliwościom Jerzy, bez dodawania atrap nieistniejących modułów.

Sprawdzono: pełne zrzuty clean/patched/generative/expert; zachowanie kliknięć po zmianie położeń gniazd; długi suwak i odpowiadającą mu gałkę; nuty oraz ostatni, 29. kontakt; rozmiary 924×644, 1320×920 i 2640×1840. Wyniki Linux: PASS. Windows również zaliczył te interakcje i natywny obraz załadowanego VST3; cały panel mieści się na pulpicie runnera po dopasowaniu. Pobrana paczka przeszła kontrolę PE, eksportów i SHA256. Wynik oceny kompozycji: PASS w zakresie układu i proporcji, z zastrzeżeniem mikrotekstu opisanym powyżej. Szczegóły w IMPLEMENTATION_STATUS.md.
