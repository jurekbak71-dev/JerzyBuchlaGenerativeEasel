# Kontrola przed upublicznieniem — 2026-10-31

Użytkownik wyraźnie zgodził się na publiczną widoczność `jurekbak71-dev/JerzyBuchlaGenerativeEasel` w celu próby użycia bezpłatnych standardowych runnerów Actions dla repozytorium publicznego.

Sprawdzono trzy zdalne gałęzie (`main`, `codex/voltage-garden-implementation`, `codex/windows-build-status`); brak zdalnych tagów. Gitleaks v8.24.3, przypięty commit `107a41827bb6698dbbff756b2300537774a1d84c`, zbudowano z oficjalnego źródła z weryfikacją zależności Go. Skan `git --log-opts=--all --redact` sprawdził 42 commity: zero wykrytych sekretów. Niezależny skan plików wyeksportowanych przez `git archive HEAD`: zero wykrytych sekretów. Raporty JSON w tym katalogu są puste i nie zawierają wartości poświadczeń.

Przegląd nazw plików w historii nie wykazał `.env`, kluczy prywatnych, certyfikatów klienta ani plików credentials. W śledzonej historii brak `.cache/`; pobrane repozytoria referencyjne i narzędzia są poza publikowanym Gitem. Sprawdzane pliki obejmują kod, wymagania, dokumentację, wygenerowane presety/audio/screenshoty i logi kompilacji/testów. Widoczna stanie się także standardowa historia autorów Git. Skan dotyczy stanu sprzed tego dokumentacyjnego commitu.

Początkowa automatyczna kontrola wstrzymała zmianę widoczności bez audytu. Audyt wykonano, a ponowione polecenie zostało dopuszczone do wykonania. Sama operacja `gh repo edit ... --visibility public` zwróciła jednak `Patch https://api.github.com/repos/...: Forbidden`. Nie potwierdzono zmiany widoczności. Wymagana jest operacja właściciela w ustawieniach GitHub lub działający autoryzowany dostęp do API ustawień.

Upublicznienie nie jest potwierdzeniem rozwiązania blokady zaległych płatności. Po zmianie widoczności trzeba sprawdzić rzeczywisty wynik nowego uruchomienia. Nie zmieniano płatności, limitów kosztów ani żadnego innego repozytorium.
