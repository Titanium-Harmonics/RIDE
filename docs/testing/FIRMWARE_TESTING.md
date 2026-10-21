# Firmware testing

This document defines the first R.I.D.E. Mk1 firmware test entry point introduced in WP-003B.

## Prerequisites

Use the existing Zephyr 4.3 development container or Codespaces environment. Run `make setup` once if the West workspace has not been initialized. No physical ESP32 board is needed for the host tests.

## Run the host tests

From the repository root:

```bash
make test
```

This runs Zephyr Twister on `tests/ci/` with the `native_sim` host platform. Twister discovers `testcase.yaml` and executes Ztest assertions. Results and logs are written under `build/twister/`. Consult Twister's generated reports and per-suite logs when a test fails. The command exits nonzero on build or test failure.

The initial suite tests Zephyr message-queue FIFO ordering, queue-full handling and queue-empty handling. A per-test hook purges the queue so test cases remain independent.

## Test categories

- **Host-executed tests:** `make test` runs the test binary on the development host using Zephyr `native_sim`. These tests can exercise Zephyr APIs but not real ESP32 peripherals.
- **Target firmware build:** `make build` compiles the current ESP32-C3 firmware. A successful build is not evidence that it ran on hardware.
- **Hardware-in-the-loop tests:** Not configured in WP-003B. Future work will define hardware execution and device-specific assertions.

## Adding test suites

Create a new subdirectory under `tests/ci/` with `CMakeLists.txt`, `prj.conf`, `testcase.yaml`, and test sources. Give each scenario a unique `ride.*` name. Constrain host-run suites to `native_sim`; do not put board-specific code into a host-only suite. Keep test setup deterministic and make failures observable through assertions.

## Troubleshooting

- If `west` or Zephyr packages cannot be found, run `make doctor` and `make setup` in the development container.
- If Twister cannot use `native_sim`, check the installed Linux host compiler and Zephyr dependencies in the development environment.
- If tests fail, inspect `build/twister/` and the printed Twister summary before changing assertions.

WP-003C will establish GitHub Actions verification and provide first observed CI results. No CI or on-target execution is claimed in this increment.

## PlantUML diagram validation (WP-003D)

Place version-controlled PlantUML diagrams in `docs/diagrams/` using the `.puml` extension. The lightweight `PlantUML syntax validation` CI job finds `.puml` and `.plantuml` files in the project (excluding downloaded West dependencies and build outputs), downloads pinned PlantUML v1.2026.8, and runs a syntax-only check. A syntax error fails the job; generated images are not committed.

The initial smoke diagram is `docs/diagrams/firmware-ci.puml`. Keep diagrams self-contained where practical. The checker is separate from the Zephyr build, so changing a diagram does not require fetching the West workspace for this check.
