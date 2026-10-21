# POC layout (planning draft)

POCs run sequentially during WP-005, WP-006, and WP-007; this draft does not contain implementation code.

```text
poc/
  wp-005-navigation/{firmware,android,tests,data,docs,README.md}
  wp-006-multicamera-ble/{firmware,android,tests,data,docs,README.md}
  wp-007-mcu-benchmark/
    firmware/{common,esp32,nordic,stm32}
    android/
    tests/
    data/
    results/
    docs/
    README.md
```

Each POC should have reproducible build/test instructions, measurements, known limitations and a feasibility verdict. WP-007 reuses artifacts from the other two POCs. Keep POC and production builds separate.
