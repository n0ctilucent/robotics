# rover Project Mandates

## Context
A C++ project for hardware control of a rover. It currently focuses on basic motor movement via direct GPIO manipulation using `libgpiod`.

## Project Stack
- **Language:** C++17 or later.
- **Build System:** Autotools (Autoconf/Automake).
- **Primary Dependency:** `libgpiod` (v1.6+). 
- *Note: While `README.md` mentions `wiringPi`, current implementations use `libgpiod` for better modern Linux compatibility.*

## Development Workflow
- **Initialization:** Use `./bootstrap.sh` or `autoreconf -i` to generate the build system.
- **Configuration:** Run `./configure` to check dependencies and generate Makefiles.
- **Building:** Use `make` to compile. The binary is output to `build/motor_test`.
- **Execution:** Hardware access often requires elevated permissions or group membership (`gpio` group).

## Engineering Standards
- **Modularization:** As the project grows, move GPIO logic into discrete classes (e.g., `MotorController`, `SensorInterface`) in `src/`.
- **Safety:** Always ensure motor-stop logic is included in destructors or signal handlers to prevent "runaway" rovers.
- **Documentation:** Maintain a clear mapping of BCM/Physical pins used in the code to assist with wiring.

## Validation
- **Hardware Verification:** Run `build/motor_test` after changes to verify pin high/low states and timing.
- **Unit Testing:** New logic should be accompanied by tests in the `test/` directory.
