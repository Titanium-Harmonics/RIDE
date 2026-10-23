#!/usr/bin/env python3
"""Build a portable ESP32-C3 flashing archive from a Zephyr build."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shlex
import subprocess
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[2]
FLASH = ROOT / "scripts" / "flash"
SUPPORTED = {"--esp-app-address", "--esp-flash-mode", "--esp-flash-freq", "--esp-flash-size"}
DEFAULTS = {"--esp-app-address": "0x10000", "--esp-flash-mode": "dio",
            "--esp-flash-freq": "40m", "--esp-flash-size": "detect"}


def extract_runner_options(path):
    """Read simple esp32 runner arguments from generated runners.yaml.

    This intentionally rejects unknown ESP32 arguments rather than creating
    an archive which could silently flash with incorrect parameters.
    """
    contents = path.read_text(encoding="utf-8")
    if not re.search(r"(?m)^\s*flash-runner:\s*esp32\s*$", contents):
        raise ValueError("Build does not use the Zephyr esp32 flash runner")
    result = dict(DEFAULTS)
    # Generated YAML commonly stores arguments as '  - --esp-foo=value'.
    tokens = re.findall(r"--esp-[a-z-]+(?:=[^\s\"',\]]+)?", contents)
    for token in tokens:
        key, sep, val = token.partition("=")
        if key in ("--esp-idf-path", "--esp-device", "--esp-baud-rate",
                   "--esp-monitor-baud"):
            continue
        if key not in SUPPORTED or not sep:
            raise ValueError(f"Unsupported ESP32 runner option: {token}")
        result[key] = val
    return result


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--build-dir", required=True, type=Path)
    p.add_argument("--output", default="dist/RIDE-ESP32C3.zip", type=Path)
    args = p.parse_args()
    build = args.build_dir.resolve()
    if not (build / "CMakeCache.txt").is_file():
        p.error("Missing CMakeCache.txt: run make pristine first")
    cache = (build / "CMakeCache.txt").read_text(errors="replace")
    if not re.search(r"(?m)^BOARD(?:_TARGET)?:[^=]*=esp32c3_devkitm\b", cache):
        p.error("This packager currently supports only ride_esp32c3")
    runner = build / "zephyr" / "runners.yaml"
    binary = build / "zephyr" / "zephyr.bin"
    if not runner.is_file() or not binary.is_file() or not binary.stat().st_size:
        p.error("Missing Zephyr runner configuration or nonempty zephyr.bin")
    try:
        opts = extract_runner_options(runner)
    except ValueError as ex:
        p.error(str(ex))
    sha = hashlib.sha256(binary.read_bytes()).hexdigest()
    git = subprocess.run(["git", "rev-parse", "--short=12", "HEAD"], cwd=ROOT,
                         capture_output=True, text=True, check=False)
    manifest = {
        "format": 1, "board": "ride_esp32c3/esp32c3", "chip": "esp32c3",
        "git_revision": git.stdout.strip() if git.returncode == 0 else "unknown",
        "esptool_version": "4.9.0",
        "image": {"filename": "zephyr.bin", "sha256": sha,
                  "address": opts["--esp-app-address"]},
        "flash_mode": opts["--esp-flash-mode"],
        "flash_freq": opts["--esp-flash-freq"],
        "flash_size": opts["--esp-flash-size"]
    }
    output = (ROOT / args.output).resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_DEFLATED) as z:
        z.write(binary, "zephyr.bin")
        z.writestr("manifest.json", json.dumps(manifest, indent=2) + "\n")
        for src, dest in [("flash.sh", "flash.sh"), ("flash.ps1", "flash.ps1"),
                          ("flash.py", "flash.py"), ("README.md", "README.md")]:
            z.write(FLASH / src, dest)
    print(f"Firmware package: {output}")
    print(f"SHA256 image: {sha}")


if __name__ == "__main__":
    main()
