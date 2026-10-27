# WP-004C — R.I.D.E. CYD board bring-up

## Goal

Run the **existing** R.I.D.E. Zephyr firmware on the ESP32-2432S028 CYD
(ESP-WROOM-32), without changing application behavior.

## Target

- Zephyr board name: `ride_cyd`
- Planned target qualifier: `ride_cyd/esp32/procpu` (verify against Zephyr 4.3.0 SoC qualifiers before implementing)
- USB-C and Micro-USB connectors on the physical board; use the enumerated USB-to-UART COM port.
- Preserve `ride_esp32c3/esp32c3` support.
- Display controller: **ST7789 assumed, unverified**; display bring-up is a later milestone.
- Touchscreen: explicitly out of scope.

## Milestone 1 — firmware parity (no LCD)

1. Add a custom board definition based on the **Zephyr 4.3.0 classic ESP32** SoC, flash layout, and UART console.
2. Build the current `app` firmware for the CYD without modifying its Shell or input logic.
3. Flash and observe Zephyr boot on USB-to-UART serial.
4. Run:
   - `ride status`
   - `ride button rec press`
   - `ride button rec release` → `[E] RECORD_TOGGLE`
   - `ride button back press`, wait >= 1 s, then `ride button back release` → `[E] UI_GO_HOME`
5. Confirm `ride_esp32c3/esp32c3` still builds.
6. Extend firmware packaging to accept both boards **and** use board-specific artifact names; avoid overwriting the ESP32-C3 package.

## Exclusions

No LCD initialization, LVGL, touch input, physical button pin mapping, camera control, or feature changes in this milestone.

## Test status

This is the acceptance specification. CYD board files and builds are **not yet implemented or executed**.
