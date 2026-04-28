<div align="center">

# ESPHOME-VICTRON

[![Release](https://img.shields.io/github/v/release/havanti/esphome-victron?style=flat-square&color=blue)](https://github.com/havanti/esphome-victron/releases) [![Stars](https://img.shields.io/github/stars/havanti/esphome-victron?style=flat-square&color=yellow)](https://github.com/havanti/esphome-victron) [![ESPHome](https://img.shields.io/badge/ESPHome-%E2%89%A52026.4.2-blue?style=flat-square)](https://esphome.io) [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg?style=flat-square)](LICENSE)

[Geräte](#unterstützte-geräte) • [Schnellstart](#schnellstart) • [Bind-Keys](#bind-keys-auslesen) • [Beispiel](#beispielkonfiguration) • [FAQ](#faq)

🇩🇪 Deutsch | [🇬🇧 English](README.en.md)

*Beiträge willkommen — bitte zuerst ein [Issue öffnen](../../issues), bevor du einen PR erstellst.*

</div>

> **Hinweis:** Dieses Repository steht in keiner Verbindung zu Victron Energy und ist
> nicht offiziell genehmigt. Nutzung auf eigene Gefahr.

---

## Schnellstart

1. **ESP32 vorbereiten** → ein BLE-fähiges Board (z.B. [M5Stack Atom Lite](https://docs.m5stack.com/en/core/ATOM%20Lite)) im Empfangsbereich der Victron-Geräte
2. **Bind-Keys auslesen** → in der VictronConnect-App pro Gerät ([Anleitung unten](#bind-keys-auslesen))
3. **YAML zusammenstellen** → siehe [Beispielkonfiguration](#beispielkonfiguration); MAC + Bind-Key über `!secret` einbinden
4. **Flashen** → `esphome run victron.yaml`

---

## Danksagungen

Großer Dank an **[Fabian Schmidt](https://github.com/Fabian-Schmidt)** für das ursprüngliche
[esphome-victron_ble](https://github.com/Fabian-Schmidt/esphome-victron_ble)-Repository.
Dieses Projekt ist ein Fork seiner Arbeit — die gesamte Protokollimplementierung,
das Decoding der einzelnen Gerätetypen und die ausführliche Dokumentation stammen von ihm.
Ohne sein Engagement gäbe es nichts davon.

Außerdem danke an Victron Energy für die [öffentliche Dokumentation des Advertising-Protokolls](https://community.victronenergy.com/questions/187303/victron-bluetooth-advertising-protocol.html).

---

## Was dieser Fork anders macht

Der Fork ist bewusst minimal-invasiv und bleibt nahe am Original:

- **ESP-IDF only** — Arduino-Framework wird abgewiesen (CLAUDE.md-Regel des Maintainers)
- **C99-Integer-Typen** durchgehend (`uint8_t` statt `u_int8_t`) — kompiliert sauber unter ESP-IDF 5.x
- **Bugfix BinarySensor**: `publish_state("")` in Default-Zweigen entfernt (löste fälschlich `true` aus)
- **Bind-Keys = Secrets**: `.git/info/exclude` schützt lokale YAMLs mit Schlüsseln vor versehentlichen Commits

Funktional unverändert gegenüber Upstream — alle Gerätetypen, Sensoren, Trigger und
Automation-Hooks aus dem Original sind erhalten.

---

## Unterstützte Geräte

| Familie | Gerätetypen |
|---|---|
| **Battery Monitor** | SmartShunt, BMV-700/702/712, Smart Battery Sense |
| **Solar Charger** | BlueSolar / SmartSolar MPPT (alle dokumentierten Modelle) |
| **Inverter** | Phoenix Inverter, Inverter RS |
| **DC/DC** | Orion XS, DC/DC Converter |
| **Charger** | Blue Smart, AC Charger |
| **Lithium / BMS** | SmartLithium, Lynx Smart BMS |
| **Multi** | Multi RS |
| **Schutz** | Smart Battery Protect |
| **Bus** | VE.Bus, DC Energy Meter |

> **Hinweis Blue Smart Charger:** Firmware ≥ v3.61, VictronConnect ≥ v6.10beta14 erforderlich.
> Vollständige Modellliste: siehe [`components/victron_ble/victron_ble.h`](components/victron_ble/victron_ble.h)
> (Enum `VICTRON_PRODUCT_ID`).

---

## Hardware

Plug & Play — keine Verkabelung. Ein BLE-fähiger ESP32 im Empfangsbereich der Victron-Geräte
genügt. Empfehlung: **M5Stack Atom Lite** — kompakt, günstig, ESP-IDF-tauglich, dient
gleichzeitig als [ESPHome Bluetooth Proxy](https://esphome.io/components/bluetooth_proxy.html)
für weitere BLE-Geräte.

---

## Bind-Keys auslesen

Pro Victron-Gerät werden zwei Werte benötigt:

- **MAC-Adresse** — Bluetooth-Adresse des Geräts
- **Bind-Key** — AES-Verschlüsselungsschlüssel (32-stelliger Hex-String)

In der [VictronConnect-App](https://www.victronenergy.com/support-and-downloads/software) (≥ v5.93):

| Schritt | Screenshot |
|---|---|
| 1. Gerät verbinden, **Settings** öffnen | <img src="img/VictronApp_00_Settings.png" width="200"> |
| 2. **Product Info** öffnen | <img src="img/VictronApp_01_ProductInfo.png" width="200"> |
| 3. **Instant readout via Bluetooth** aktivieren, **SHOW** unter *Encryption data* drücken | <img src="img/VictronApp_03_EncryptionData.png" width="200"> |

> **Wichtig:** Solange das Gerät über VictronConnect verbunden ist, sendet es **keine**
> Advertisements. Nach Auslesen der Daten App schließen / Verbindung trennen.

> **Bind-Key falsch?** Bei `victron_ble:XXX incorrect bindkey` prüfen, ob exakt 32 Zeichen
> kopiert wurden — die App bricht den Schlüssel manchmal in einer unsichtbaren zweiten
> Zeile um. Über die Zwischenablage-Schaltfläche kopieren. Bei nur 31 Zeichen muss der
> Bluetooth-PIN am Gerät zurückgesetzt werden.

---

## Beispielkonfiguration

Vollständiges Beispiel: [`victron_ble.yaml`](victron_ble.yaml). Minimal:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/havanti/esphome-victron
    refresh: 24h

esp32:
  board: m5stack-atom
  framework:
    type: esp-idf

esp32_ble_tracker:

victron_ble:
  - id: SmartShunt1
    mac_address: !secret victron_smartshunt_mac
    bindkey: !secret victron_smartshunt_bindkey

sensor:
  - platform: victron_ble
    victron_ble_id: SmartShunt1
    name: "SmartShunt Battery Voltage"
    type: BATTERY_VOLTAGE
  - platform: victron_ble
    victron_ble_id: SmartShunt1
    name: "SmartShunt Battery Current"
    type: BATTERY_CURRENT
```

`secrets.yaml`:

```yaml
victron_smartshunt_mac: "AA:BB:CC:DD:EE:FF"
victron_smartshunt_bindkey: "0123456789abcdef0123456789abcdef"
```

---

## Voraussetzungen

- **ESP32** (Classic, S3, C3 oder C6 — alles mit Bluetooth LE)
- **Framework:** ESP-IDF (Arduino wird abgewiesen)
- **ESPHome** ≥ 2026.4.2
- **VictronConnect-App** ≥ 5.93 zum Auslesen der Bind-Keys

---

## FAQ

### Welche Sensoren sind für mein Gerät verfügbar?

Jedes Victron-Gerät sendet genau einen Record-Typ. Die unterstützten `type:`-Werte
pro Record sind in [`components/victron_ble/sensor/__init__.py`](components/victron_ble/sensor/__init__.py)
definiert. Anhaltspunkt: was die VictronConnect-App auf der Übersichtsseite (vor dem
Verbinden) zeigt, kommt auch im Advertisement.

### Wie begrenze ich die Update-Rate?

ESPHome-Standard-Filter:

```yaml
- platform: victron_ble
  victron_ble_id: SmartShunt1
  name: "Battery Current"
  type: BATTERY_CURRENT
  filters:
    - throttle_average: 60s
    - timeout: 120s
```

Siehe [ESPHome Sensor Filters](https://esphome.io/components/sensor/index.html#sensor-filters).

### Warum kommen keine Daten?

- VictronConnect-App muss vom Gerät getrennt sein (App schickt sonst keine Advertisements)
- Bind-Key exakt 32 Zeichen?
- ESP32 in Reichweite (RSSI in Logs prüfen, `logger: level: DEBUG`)
- `esp32_ble_tracker` aktiviert?

---

## Lizenz

GPL-3.0 — siehe [LICENSE](LICENSE). Original-Lizenz von Fabian Schmidt übernommen.
