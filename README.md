# R.I.D.E. Mk1

**Real-time Intelligent Detector of Events**

R.I.D.E. is an open-source motorcycle-mounted smart controller developed by Titanium Harmonics.

It aims to combine navigation, action-camera control, trip telemetry, and motorcycle security into one compact device, supported by an Android companion app.

## Project Features

R.I.D.E. is being developed around five main capabilities:

- **Navigation:** Turn-by-turn instructions using predefined arrows and symbols.
- **Camera Control:** Control up to three action cameras, including GoPro, Insta360, and DJI.
- **Trip Dynamics:** Record ride telemetry and events, generating trip files for later video editing.
- **Guard Mode:** Monitor motorcycle movement and detect potential interference while parked.
- **User Interface:** On-device display and controls, supported by an Android companion app.

**Current status:** Early development. The firmware development environment and ESP32-C3 build/flashing workflow are established. The main product features remain under development.

## Getting Started

The firmware uses **Zephyr RTOS** and currently targets the **ESP32-C3 Super Mini** using the project-local `ride_esp32c3/esp32c3` Zephyr board.

The recommended development environment is **GitHub Codespaces**, which provides the required tools through a preconfigured Dev Container.

1. Open this repository in GitHub Codespaces using the `development` branch.
2. Wait for the automatic environment setup to finish.
3. Open a terminal and run:

```bash
make doctor
make build
```

For a clean rebuild:

```bash
make pristine
```

Alternatively, clone the repository using VS Code with Docker and the Dev Containers extension. Run `make setup` if the environment hasn't been initialized.

## Firmware Download & Flashing

To package the compiled ESP32-C3 firmware for flashing on your computer:

```bash
make fw-download
```

Open the generated download link and extract the ZIP file on your computer.

Flash the ESP32-C3 using the included script:

**Linux:**
```bash
bash flash.sh --port /dev/ttyACM0
# or: chmod +x flash.sh && ./flash.sh --port /dev/ttyACM0
# USB-to-UART adapter: use /dev/ttyUSB0 instead
```

**Windows PowerShell:**
```powershell
.\flash.ps1 -Port COM5
```

See [scripts/flash/README.md](scripts/flash/README.md) for additional flashing instructions.

---

Part of [Titanium Harmonics](https://titanium-harmonics.com).
