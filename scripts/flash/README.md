# R.I.D.E. ESP32-C3 local flashing

The ZIP is produced by `make fw-package` or `make fw-download` in Codespaces.
Extract **all** ZIP contents into one directory before flashing. No Zephyr SDK
is needed locally. Python 3 and internet access are required on first use.
The scripts create a local `.venv` and install the pinned `esptool` version
automatically, then reuse it.

Linux:

```sh
bash flash.sh --port /dev/ttyACM0
# or: chmod +x flash.sh && ./flash.sh --port /dev/ttyACM0
# USB-to-UART adapter: use /dev/ttyUSB0 instead
```

Windows PowerShell:

```powershell
.\\flash.ps1 -Port COM5
```

Omit the port only when exactly one serial port is connected. The script
validates the SHA256 checksum, identifies the port, and asks permission before
flashing. Override the baud rate with `--baud 115200` on Linux or `-Baud
115200` on Windows if necessary.

Linux USB access may require group membership or udev permissions. Disconnect
other serial applications before flashing. Some ESP32-C3 native-USB boards may
require manually entering bootloader mode or cycling power after flashing.

The ZIP supports the R.I.D.E. single-image ESP32-C3 Zephyr build only. Builds
with additional bootloader or partition images must use a future packaging
format; do not reuse this package for other boards.
