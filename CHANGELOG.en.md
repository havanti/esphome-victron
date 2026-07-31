# Changelog

[🇩🇪 Deutsch](CHANGELOG.md) | 🇬🇧 English

All notable changes to this project are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

---

## Compatibility status

Tested with:
- ESPHome **2026.7.3** — ESP-IDF ✅
- ESPHome **2026.6.5** — ESP-IDF ✅

Platform: ESP32 with ESP-IDF framework only (no Arduino).

---

## [0.1.2] — 2026-07-17 — Correctness fixes from code audit

### Changed
- `victron_ble`: bindkey mismatch log lowered from `ESP_LOGW` to `ESP_LOGV` — byte 0 of the bindkey no longer appears in standard logs
- `victron_ble` (`__init__.py`): codegen declaration corrected from `cg.PollingComponent` to `cg.Component` (the C++ class inherits `Component`; `update()` is called manually)

### Fixed
- `victron_scanner`: `VICTRON_BT_MANUFACTURER_DATA` marked `__attribute__((packed))` and record type/version (0x10/0x02) validated before use — prevents misinterpreting foreign manufacturer data
- `victron_ble_connect`: `handle_keep_alive_` initialised to `0` — was undefined until the first `SEARCH_CMPL` event

## [0.1.1] — 2026-04-29 — Cross-task synchronisation + cleanup

### Changed
- `victron_ble`: `last_package_updated_` converted to `std::atomic<bool>`; `update()` uses `exchange(false)` for atomic test-and-clear
- `victron_ble_connect`: 10 sets of value/flag members (`value_*`, `value_is_set_*`, `request_read_*`, `handle_*`) converted to `std::atomic` — prevents race conditions between BLE GATT task and main loop

### Removed
- `victron_ble`: obsolete config key `submit_sensor_data_asap` removed from schema (was marked `cv.invalid`)


## [0.1.0] — 2026-04-28 — Initial Havanti fork

### Added
- Standalone fork of [Fabian-Schmidt/esphome-victron_ble](https://github.com/Fabian-Schmidt/esphome-victron_ble) as `havanti/esphome-victron`
- Bilingual documentation (DE/EN) in Havanti style: `README.md`, `README.en.md`, `CHANGELOG.md`, `CHANGELOG.en.md`

### Changed
- Strict ESP-IDF platform check: C++ guard `USE_ESP32_FRAMEWORK_ESP_IDF`, Python validator `CORE.is_esp32 and not CORE.using_arduino`
- Replaced all POSIX `u_int*_t` types with C99 `uint*_t` (POSIX types are not provided by GCC under ESP-IDF 5.x)
- `encrypted_data` signatures now consistently use the `VICTRON_ENCRYPTED_DATA_MAX_SIZE` constant
- `victron_ble.yaml` example: source set to `havanti/esphome-victron`, framework explicitly `esp-idf`, `min_version: 2026.4.3`
- `CODEOWNERS` of all components set to `@havanti`

### Fixed
- BinarySensor: removed `publish_state("")` in default branches — the string literal was implicitly converted to `bool true` and falsely triggered alarm/error states

### Removed
- Upstream maintainer infrastructure not useful for this fork: `.github/`, `.devcontainer/`, `.clang-format`, `.clang-tidy`, `.flake8`, `.yamllint`, `script/`, `tests/`, `platformio.ini`, `archive/`
- WT32-SC01 display example (Arduino framework, incompatible with ESP-IDF-only)
- Standalone `README.victron_ble_connect.md` (content will be re-created as bilingual docs when needed)

### Documentation
- New `README.md` in Havanti style with quickstart, device table, bind-key guide, sample configuration and FAQ
- Notes on bind-key security (exclude local YAMLs via `.git/info/exclude`)
