# R.I.D.E. Mk1

**Real-time Intelligent Detector of Events** — a motorcycle controller for action cameras, ride event tagging, telemetry, and BLE connectivity. Navigation and multi-camera features are planned; the firmware currently prints a UART heartbeat.

## GitHub Codespaces (recommended cloud development)

1. Open the [RIDE repository](https://github.com/Titanium-Harmonics/RIDE) on GitHub.
2. Select **Code → Codespaces → New with options**, choose branch `build/reproducible-dev-environment` while this PR is under review (or the merged target branch later), and create a Codespace. Select an **x86-64 Linux** machine; **4 cores / 16 GB RAM** is a practical starting point if available.
3. GitHub builds the same `.devcontainer` image used by desktop VS Code. The **post-create** command automatically runs `make setup` once to initialize the Zephyr workspace and download ESP32 blobs.
4. Open the terminal and run:

```sh
make doctor
make pristine
```

### Codespaces troubleshooting

If you created a Codespace on the repository's default `development` branch and **then checked out the WP1 branch**, the running Codespace may still use GitHub's default image rather than this branch's `.devcontainer`. Changing branches does **not** rebuild the container automatically.

Run **Codespaces: Rebuild Container** (or **Dev Containers: Rebuild Container**) from the VS Code Command Palette and inspect the rebuild log. After successful container creation:

```sh
whoami             # ubuntu
command -v west     # /home/ubuntu/venv/bin/west
west --version
make doctor
make pristine
```

A missing `west` command **before** setup indicates that the intended Docker image has not been activated or its build failed. Rebuilding is preferred over manually installing tools into the fallback environment.

If `west` exists but the post-create step fails (e.g. transient network failure), inspect the creation log and retry `make setup` manually. Future workspaces can be created directly from the branch; no local Docker or WSL is needed.

**Limitations:** Codespaces compiles firmware and, after WP2, will run host unit/integration tests. It cannot directly access a USB board plugged into your laptop. Download build artifacts or flash locally; remote hardware-in-the-loop testing requires a separate test runner. Creating and running Codespaces consumes your GitHub Codespaces quota.

## Development environment

- Docker and VS Code with the Dev Containers extension (Linux, WSL2, or Docker-supported desktop).
- An x86-64 Linux Docker host is required for the currently installed Zephyr SDK archive.
- Zephyr **v4.3.0** is pinned in `app/west.yml`; Zephyr SDK **0.17.4** and west **1.5.0** are selected in `.devcontainer/Dockerfile`.
- Board: `esp32c3_devkitm/esp32c3`. Other Zephyr targets can be selected using `BOARD=...`.
- After cloning, select **Dev Containers: Reopen in Container** in VS Code, then run:

```sh
make setup
make doctor
make build
```

`make setup` downloads the Zephyr modules, ESP32 RF blobs, and matching Python packages. First setup requires internet access and disk space. Re-running setup after module updates is supported.

For a clean build:

```sh
make pristine
```

To flash an attached ESP32-C3 (USB passthrough must be configured on your host):

```sh
make flash
```

Change target with `make build BOARD=<zephyr-board-target>`. Each target uses a separate build directory under `build/`.

## Portable firmware download and local flashing

Codespaces cannot reach a USB board attached to your laptop. After `make pristine`:

```sh
make fw-package      # Create dist/RIDE-ESP32C3.zip
make fw-download     # Package, then serve a private browser download on port 8765
```

With `fw-download`, use the terminal link or the Codespaces **Ports** tab
to open port 8765, keeping its visibility **Private**. The temporary server shuts down automatically after sending the ZIP, or after\n120 seconds without a completed download. The browser page attempts to close itself,\nbut browsers generally prevent closing tabs opened manually, so you might need\nto close the tab yourself. Save and extract the ZIP on your laptop.

Linux: `bash flash.sh --port /dev/ttyUSB0`

Windows PowerShell: `.\\flash.ps1 -Port COM5`

Python 3 is the only prerequisite; the scripts create a local virtual
environment and install a pinned esptool version if missing. The flashing
process checks the firmware SHA256, chooses a serial port only if a single
candidate exists, and requests confirmation before writing. See the
`scripts/flash/README.md` bundled in the archive.

**Currently supported:** single-image ESP32-C3 `esp32c3_devkitm/esp32c3`
builds using the Zephyr ESP32 runner. The packager reads generated flash
configuration from `runners.yaml` and rejects unsupported runner options.
The process has not yet been validated on a physical board.

## Local checks

```sh
make format-check
make lint
```

These are static checks only. Host unit tests, integration tests, and GitHub Actions will be added in WP2/WP3; **WP1 does not claim test or CI coverage**.

## Reproducibility notes

The Zephyr manifest uses the pinned Zephyr release and its upstream module revisions. The SDK and west versions are selected explicitly; Ubuntu packages, Python transitive dependencies and the Docker base image are not immutable, so this is a version-controlled development baseline rather than a bit-for-bit reproducible build. `west packages pip --install` resolves Zephyr's requirements and may change the installed west version when Zephyr requires it.

## Repository conventions

Follow Titanium Harmonics' private `internal-docs` standards. Use a focused branch, PR, and verified checks before merge. The existing `development` default branch is retained for now; the organization-standard `main` transition is a separate change.

## Roadmap

Planned: BLE camera control (GoPro, Insta360, DJI), multiple camera links, FOTA, IMU event detection, GPS logging and Android companion/navigation integration.
