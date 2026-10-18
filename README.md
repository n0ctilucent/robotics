# Robotics Workspace

## Projects in This Repository

### 1. ESP32 Badge Hardware Suite
**Status:** Design phase — firmware and host tooling under development.

ESP32-WROOM-32E badge hardware, inter-badge BLE comms protocol (DC25 spec), and the `hardware-agent` skill suite running on **chonk** (`10.10.8.60`).

**Key Components:**
- **Hardware Agent Skill Suite** — Located at `~/.openclaw/workspace/skills/hardware-agent/`
  - `serial-bridge/` — UART control gateway for ESP32 badges (control.py, monitor.py, gateway.py, discover.py)
  - `firmware-builder/` — PlatformIO-based firmware build automation (fbuild.py)
  - `io-scanner/` — I2C/GPIO enumeration and scanning utilities
  - `badge-mqtt-bridge/` — MQTT bridge for badge telemetry (bridge.py)
- **Badge-to-Badge BLE Spec** — `docs/DC25BadgetoBadgeSpec.odt` (DC25 Confidential)
- **ESP32 WROOM-32E Datasheet** — `docs/esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf`
- **ESP32 Dev Breakout BOM** — `docs/esp32-dev-breakout-generated-bom.ods` (23 rows: ESP32 module, resistors, capacitors, tactile switches, LEDs, JST connectors, LM1117-3.3 regulator)

**Host Environment (chonk):**
- Python 3.13.5, Node v24.22.0, Mosquitto MQTT on port 1883
- ESP32 target at `/dev/ttyUSB0`, 115200 baud
- Dependencies: `paho-mqtt` (installed), `esptool.py`, `platformio-core` (pending install)

**MQTT Topic Convention:**
```
badge/{hostname}/{component}/{action}
Examples:
  badge/chonk/uart/log
  badge/chonk/led/set
  badge/chonk/i2c/scanned
  badge/chonk/gpio/<pin>/state
  badge/chonk/firmware/version
  badge/chonk/button/pressed
```

**Constraints:**
- SAO bus draws from badge 3V3 rail — limit to 4 simultaneous SAO modules without power confirmation
- I2C scans block other I2C operations for ~1-2 seconds
- ESP32 default SPI (MOSI=IO23, MISO=IO19, SCK=IO18) conflicts with APA102 LEDs; verify pin availability
- QoS 1 + retain=true for firmware version/GPIO state; QoS 0 for high-frequency telemetry

---

### 2. 4-Wheel Robot Car (Bluetooth Controlled)
**Status:** Analysis complete — architectural patterns identified for badge integration.

ESP32-based Bluetooth-controlled 4-wheel robot car platform (`github.com/crashbandicoot19/ESP32_Bluetooth_Controlled_4_Wheel_Robot_Car`). Reviewed for adaptive patterns:
- Unified command protocol (plaintext shortcuts mapped to JSON payloads)
- Event-driven telemetry relay architecture
- Modular hardware profiles / board definitions

**Pattern Adaptations for Badge Agent:**
- Command aliases (e.g., `badge scan` → `{"cmd":"scan_i2c"}`)
- Button press events published as instant MQTT (`badge/chonk/button/pressed`)
- Board Profile JSON format in `skills/hardware-agent/profiles/`

---

### 3. Robot Chassis Assembly (DG012-BV Multi-Chassis 4WD Kit)
**Status:** Documentation only — image-based PDF, no editable assembly instructions available.

Parts: aluminum chassis bottom, M3 flat-head screws (6x), M2.5 nylon screws (8x), distance holders, spanners (M2.5 & M4, M2 & M3), 5xAA battery holder, 4x gear box assemblies.

Assembly PDF (`robot-chassis.pdf`) is image-based; OCR extraction yielded only partial text from the parts list page. Full assembly instructions require physical reference or manual transcription.

---

### 4. Robotics Applications Notes
**Status:** Ideation — project notes from Frank Diaz outlining conceptual robot applications.

From `project_notes.docx`:
- Manual RC-car-style control via web interface on BBBone (initial)
- Autonomous obstacle detection and area mapping (reference: uw.edu robotics mapping projects)
- "Hack" platform: BBB controlling Raspberry Pi wireless pentesting suite (GPS, SDR, Bluetooth, Wi-Fi)
- Kismet map generation with Google Earth visualization
- Semiotics / "Chinese Room" problem exploration using maritime signal flags and machine vision

---

### 5. Kolban's ESP32 Reference
**Status:** Reference library — `docs/kolban-ESP32.pdf` (comprehensive ESP32 technical reference).

---

## Directory Structure
```
robotics/
├── README.md                          # This file
├── GPIO.png                         # Raspberry Pi B+ J8 pinout diagram
├── Max_Control_4.apk                # Android control app binary
├── notes.txt                        # Architecture analysis / pattern review
├── project_notes.docx               # Robotics applications ideation
├── requirements.txt                 # Python dependencies (paho-mqtt)
├── 4_Channel_instruction_manual.pdf # Motor controller manual
├── robot-chassis.pdf               # Chassis assembly instructions
├── Robot project.doc                # Legacy Word 8.0 document
└── docs/
    ├── DC25BadgetoBadgeSpec.odt     # Badge BLE comm protocol spec
    ├── esp32-dev-breakout-generated-bom.ods
    ├── esp32-wroom-32e_esp32-wroom-32ue_datasheet_en.pdf
    ├── kolban-ESP32.pdf            # ESP32 technical reference
    └── rev20170621.pdf              # Historical revision (unverified content)
```

## Pending Tasks
- [ ] Install `esptool.py` and `platformio-core` on chonk
- [ ] Flash ESP32 badge RPC firmware to `/dev/ttyUSB0`
- [ ] Execute round-trip UART command test
- [ ] Create Board Profile JSON configurations in `hardware-agent/profiles/`
- [ ] Write C++ RPC firmware sketch for badge intercom protocol
