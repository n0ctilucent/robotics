# Architecture Notes

## Hardware Agent Suite — Architectural Ideas from Robot Car Project

Based on review of the ESP32 Bluetooth Controlled 4-Wheel Robot Car repository and existing badge architecture on **chonk**, here are patterns to adapt:

### 1. Unified Command Protocol

**From robot car:** Simple plaintext or JSON command strings over Bluetooth Serial (`F` = Forward, `B` = Backward, `L` = Left, `R` = Right, `S` = Stop).

**For badge agent:** Extend serial-bridge (`control.py`) to support quick macro shortcuts alongside full JSON payloads. Typing raw JSON every time gets tedious:
- `badge set_led 0 255 0 0` → `{"cmd":"set_led","index":0,"r":255,"g":0,"b":0}`
- `badge scan` → `{"cmd":"scan_i2c"}`

### 2. Event-Driven Telemetry Relay

**From robot car:** State changes from hardware transmitted back to controller.

**For badge agent:** Expand `badge-mqtt-bridge` (`bridge.py`) topic hierarchy (`badge/chonk/{component}/{action}`):
- Publish button clicks (`SW1`, `SW2` tactile switches) as MQTT events (`badge/chonk/button/pressed`)
- Relay battery voltage readings or sensor states so monitoring agents can react without manual polling

### 3. Modular Hardware Profiles / Board Definitions

**From robot car:** Clear separation of wiring configurations and motor/pin mappings.

**For badge agent:** Formalize **Board Profile JSON files** inside `skills/hardware-agent/profiles/`:

```json
{
  "board_name": "voodoo-esp32-badge",
  "mcu": "ESP32-WROOM-32E",
  "pins": {
    "apa102_data": 23,
    "apa102_clock": 18,
    "i2c_sda": 21,
    "i2c_scl": 22
  },
  "power_budget": {
    "max_led_brightness_pct": 20,
    "usb_safe": true
  }
}
```

The `firmware-builder` and `io-scanner` can read these profiles dynamically for pin testing and safe LED brightness limits.

---

## Document Inventory

| Document | Type | Location | Summary |
|---|---|---|---|
| `robot-chassis.pdf` | PDF (image) | root | DG012-BV Multi-Chassis 4WD kit parts list and assembly instructions |
| `project_notes.docx` | DOCX | root | Robotics applications ideation — manual control, autonomous mapping, pentesting platform concepts |
| `notes.txt` (consolidated) | TXT | → `docs/architecture_notes.md` | This file. Architecture analysis + badge integration ideas |
| `docs/DC25BadgetoBadgeSpec.odt` | ODT | docs | Badge-to-Badge BLE comms protocol — GAP advertising format, company identifiers, appearance codes |
| `docs/esp32-dev-breakout-generated-bom.ods` | ODS | docs | ESP32 breakout BOM — 23 rows: ESP32-WROOM-32E module, resistors, capacitors, tactile switches, LEDs, JST connectors, LM1117-3.3 regulator |
| `docs/esp32-wroom-32e_datasheet_en.pdf` | PDF | docs | Espressif ESP32-WROOM-32E datasheet (reference) |
| `docs/kolban-ESP32.pdf` | PDF | docs | Comprehensive ESP32 technical reference (reference) |
| `docs/rev20170621.pdf` | PDF (KiCad sch) | docs | Karl Koscher's Crypto and Privacy Village Badge 2017 — LED chain schematic (Sheet 2/2, Rev C) |
| `GPIO.png` | PNG | root → images/ | Raspberry Pi B+ J8 header pinout diagram |
| `4_Channel_instruction_manual.pdf` | PDF (image) | docs | Rover 5 motor controller manual — 4x FET H-bridges, quad encoder mixing circuit, current sensing |
| `Robot project.doc` | DOC (legacy) | root → docs/ | Legacy binary format — cannot parse without specialized tools |
| `docs/images/L293D*.jpg` | JPG | docs/images/ → resources/media/ | L293D motor driver reference images (pinout, block diagram) |
| `docs/images/README.md` | MD | docs/images/ | Placeholder — can be removed after move |

---

## Recommended Next Steps on chonk

1. Plug ESP32 target into `chonk` USB port
2. Run `python3 skills/hardware-agent/serial-bridge/discover.py` to confirm device node
3. Flash RPC responder sketch and test first round-trip command
