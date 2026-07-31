# Changelog

🇩🇪 Deutsch | [🇬🇧 English](CHANGELOG.en.md)

Alle wesentlichen Änderungen an diesem Projekt werden in dieser Datei dokumentiert.

Das Format basiert auf [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## Kompatibilitätsstatus

Getestet mit:
- ESPHome **2026.7.3** — ESP-IDF ✅
- ESPHome **2026.6.5** — ESP-IDF ✅

Plattform: ausschließlich ESP32 mit ESP-IDF-Framework (kein Arduino).

---

## [0.1.2] — 2026-07-17 — Korrektheits-Fixes aus Code-Audit

### Geändert
- `victron_ble`: Bindkey-Mismatch-Log von `ESP_LOGW` auf `ESP_LOGV` gesenkt — Byte 0 des Bindkeys landet nicht mehr in Standard-Logs
- `victron_ble` (`__init__.py`): Codegen-Deklaration von `cg.PollingComponent` auf `cg.Component` korrigiert (C++-Klasse erbt `Component`; `update()` wird manuell aufgerufen)

### Behoben
- `victron_scanner`: `VICTRON_BT_MANUFACTURER_DATA` mit `__attribute__((packed))` versehen und Record-Type/-Version (0x10/0x02) vor Auswertung validiert — verhindert Fehlinterpretation fremder Manufacturer-Daten
- `victron_ble_connect`: `handle_keep_alive_` mit `0` initialisiert — war bis zum ersten `SEARCH_CMPL`-Event undefiniert

## [0.1.1] — 2026-04-29 — Cross-Task-Synchronisation + Cleanup

### Geändert
- `victron_ble`: `last_package_updated_` auf `std::atomic<bool>` umgestellt; `update()` nutzt `exchange(false)` für atomares Test-and-Clear
- `victron_ble_connect`: 10 Sätze Werte-/Flag-Member (`value_*`, `value_is_set_*`, `request_read_*`, `handle_*`) auf `std::atomic` umgestellt — verhindert Race Conditions zwischen BLE-GATT-Task und main loop

### Entfernt
- `victron_ble`: obsoleter Konfig-Schlüssel `submit_sensor_data_asap` aus Schema entfernt (war als `cv.invalid` markiert)


## [0.1.0] — 2026-04-28 — Initialer Havanti-Fork

### Hinzugefügt
- Eigenständiger Fork von [Fabian-Schmidt/esphome-victron_ble](https://github.com/Fabian-Schmidt/esphome-victron_ble) als `havanti/esphome-victron`
- Zweisprachige Dokumentation (DE/EN) im Havanti-Stil: `README.md`, `README.en.md`, `CHANGELOG.md`, `CHANGELOG.en.md`

### Geändert
- Strikte ESP-IDF-Plattformprüfung: C++-Guard `USE_ESP32_FRAMEWORK_ESP_IDF`, Python-Validator `CORE.is_esp32 and not CORE.using_arduino`
- Alle POSIX-`u_int*_t`-Typen durch C99-`uint*_t` ersetzt (POSIX-Typen werden vom GCC unter ESP-IDF 5.x nicht bereitgestellt)
- `encrypted_data`-Signaturen verwenden jetzt einheitlich die Konstante `VICTRON_ENCRYPTED_DATA_MAX_SIZE`
- `victron_ble.yaml`-Beispiel: Quelle auf `havanti/esphome-victron`, Framework explizit auf `esp-idf`, `min_version: 2026.4.3`
- `CODEOWNERS` aller Komponenten auf `@havanti`

### Behoben
- BinarySensor: `publish_state("")` in Default-Zweigen entfernt — der String-Literal wurde implizit zu `bool true` konvertiert und löste fälschlich Alarm- bzw. Fehlerzustände aus

### Entfernt
- Upstream-Maintainer-Infrastruktur ohne Nutzen für diesen Fork: `.github/`, `.devcontainer/`, `.clang-format`, `.clang-tidy`, `.flake8`, `.yamllint`, `script/`, `tests/`, `platformio.ini`, `archive/`
- WT32-SC01-Display-Beispiel (Arduino-Framework, nicht mit ESP-IDF-only kompatibel)
- Eigenständige `README.victron_ble_connect.md` (Inhalt wird bei Bedarf später als zweisprachige Doku neu erstellt)

### Dokumentation
- `README.md` neu im Havanti-Stil mit Schnellstart, Geräte-Tabelle, Bind-Key-Anleitung, Beispielkonfiguration und FAQ
- Hinweise zur Bind-Key-Sicherheit (lokale YAMLs via `.git/info/exclude` ausschließen)
