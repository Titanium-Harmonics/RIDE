#!/usr/bin/env python3
"""Shared local flasher. Creates an isolated Python venv and installs esptool."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import venv

HERE = Path(__file__).resolve().parent


def main():
    ap = argparse.ArgumentParser(description="Flash R.I.D.E. ESP32-C3")
    ap.add_argument("--port", help="Serial port, e.g. COM5 or /dev/ttyUSB0")
    ap.add_argument("--baud", type=int, default=460800)
    ap.add_argument("--yes", action="store_true", help="Skip device confirmation")
    a = ap.parse_args()
    data = json.loads((HERE / "manifest.json").read_text())
    if data.get("format") != 1 or data.get("chip") != "esp32c3":
        ap.error("Unsupported package manifest")
    info = data["image"]
    image = HERE / info["filename"]
    if info["filename"] != "zephyr.bin" or not image.is_file():
        ap.error("Expected zephyr.bin is missing")
    if hashlib.sha256(image.read_bytes()).hexdigest() != info["sha256"]:
        ap.error("Firmware checksum mismatch, refusing to flash")
    env = HERE / ".venv"
    py = env / ("Scripts/python.exe" if os.name == "nt" else "bin/python")
    if not py.exists():
        print("Creating local Python environment...")
        venv.EnvBuilder(with_pip=True).create(env)
    installed = subprocess.run([str(py), "-m", "pip", "show", "esptool"],
                               capture_output=True, text=True)
    version = data["esptool_version"]
    if installed.returncode or f"Version: {version}" not in installed.stdout:
        print(f"Installing esptool {version} into local environment...")
        subprocess.run([str(py), "-m", "pip", "install", "--disable-pip-version-check",
                        f"esptool=={version}"], check=True)
    port = a.port
    if not port:
        result = subprocess.run([str(py), "-c",
            "from serial.tools.list_ports import comports; "
            "print('\\n'.join(p.device for p in comports()))"],
            capture_output=True, text=True, check=True)
        ports = result.stdout.splitlines()
        if not ports:
            ap.error("No serial ports detected; specify --port after connecting board")
        if len(ports) > 1:
            ap.error("Multiple serial ports found: " + ", ".join(ports) +
                     "; specify --port")
        port = ports[0]
    print(f"Board: {data['board']} | Port: {port} | Commit: {data['git_revision']}")
    print(f"Image address: {info['address']} | SHA256: {info['sha256']}")
    if not a.yes and input("Flash this device? [y/N] ").strip().lower() != "y":
        print("Cancelled.")
        return
    cmd = [str(py), "-m", "esptool", "--chip", data["chip"], "--port", port,
           "--baud", str(a.baud), "--before", "default_reset",
           "--after", "hard_reset", "write_flash", "-z",
           "--flash_mode", data["flash_mode"], "--flash_freq", data["flash_freq"],
           "--flash_size", data["flash_size"], info["address"], str(image)]
    subprocess.run(cmd, check=True)
    print("Flash completed.")


if __name__ == "__main__":
    try:
        main()
    except (OSError, subprocess.CalledProcessError, ValueError, KeyError) as e:
        sys.exit(f"ERROR: {e}")
