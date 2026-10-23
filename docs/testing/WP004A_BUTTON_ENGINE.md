# WP-004A — Button engine

Standalone, Zephyr-independent first increment.

## Run tests

`sh tests/button_engine/run.sh`

Uses a host C compiler and AddressSanitizer/UBSan. Does not require Zephyr.

## Source files

- `app/include/ride_buttons.h`
- `app/src/ride_buttons.c`
- `tests/button_engine/test_buttons.c`
- `tests/button_engine/run.sh`

## Boundaries

- Input adapters (GPIO, keyboard, mouse, terminal) are not implemented yet.
- Press detector assumes debounced edges; GPIO debounce must be provided by an input adapter or common preprocessing layer.
- Event callbacks should enqueue events, not execute blocking application commands.
- Button mapping is context-neutral. UI navigation resolves `NAV_PREVIOUS`/`NAV_NEXT` by screen context.
- `ACTION` short and long mappings are configurable in memory; persistent storage is deferred.
- `long_press_ms` defaults to 800 ms; this threshold is provisional.
- The engine is compiled into the Zephyr application for ESP32-C3 and native_sim; the main application does not invoke it yet.
- CI runs the host button-engine regression test before Twister and ESP32-C3 build.
- Run `sh tests/button_engine/run.sh` for fast host testing, `make build` for ESP32-C3, or `make BOARD=native_sim build` for a headless native_sim build.

## Zephyr Ztest / Twister

The canonical firmware unit tests live in `tests/ci/button_engine/` and run on `native_sim`:

```sh
west twister -T tests/ci -p native_sim --inline-logs --outdir build/twister
```

The standalone `tests/button_engine/run.sh` remains as an optional fast
AddressSanitizer/UndefinedBehaviorSanitizer check, not as a substitute for Ztest.
`make test` and firmware CI run both checks.

The Ztest suite covers the default mappings, release-only short/long actions,
ACTION customization, exclusive button ownership, ignored overlapping presses,
and 32-bit timestamp rollover.
