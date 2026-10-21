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

GitHub Actions runs ESP32-C3 firmware builds, formatting and cppcheck checks, and the native_sim Ztest suite on pull requests and pushes to `development`. The firmware workflow caches its Docker toolchain, West workspace and compiler objects; `west update` still runs on every CI job. The passing cached run [#39612449953](https://github.com/Titanium-Harmonics/RIDE/actions/runs/39612449953) demonstrated 2/2 passing host tests and 98/99 cacheable compilation hits. Hardware-in-the-loop execution remains out of scope.

## PlantUML diagram validation (WP-003D)

Place version-controlled PlantUML diagrams in `docs/diagrams/` using the `.puml` extension. The lightweight `PlantUML syntax validation` CI job finds `.puml` and `.plantuml` files in the project (excluding downloaded West dependencies and build outputs), downloads pinned PlantUML v1.2026.8, and runs a syntax-only check. A syntax error fails the job; generated images are not committed.

The initial smoke diagram is `docs/diagrams/firmware-ci.puml`. Keep diagrams self-contained where practical. The checker is separate from the Zephyr build, so changing a diagram does not require fetching the West workspace for this check.

## Pre-commit and CI review convention (WP-003F)

1. Work on a feature branch and keep its pull request in draft while checks are being developed.
2. Validate changed YAML, shell, C, Python or PlantUML locally whenever the required tools are available. If the toolchain is unavailable, label the check **not locally executed** rather than claiming it passed.
3. Run `make format-check`, `make lint`, `make test`, and `make build` inside the configured dev container for relevant firmware changes. Review logs, not just exit codes.
4. Push only after local checks, then verify the GitHub Actions workflow and artifact outputs. A green compile is not an on-device test.
5. Require explicit review and approval before merging; do not silently merge draft work.

## WP-003 acceptance evidence and limits

- **Host tests:** Twister `native_sim` Ztest suite; 2/2 executed cases passed in [run #39612449953](https://github.com/Titanium-Harmonics/RIDE/actions/runs/39612449953).
- **Firmware CI:** ESP32-C3 build, formatter, linter and artifact upload passed in the same run; Docker, West and ccache cache restoration verified.
- **Diagram checking:** Separate PlantUML syntax job including Graphviz passed in [run #39615638440](https://github.com/Titanium-Harmonics/RIDE/actions/runs/39615638440).
- **SBOM:** Four SPDX 2.3 JSON reports plus artifacts verified in [run #39617826449](https://github.com/Titanium-Harmonics/RIDE/actions/runs/39617826449); its temporary PR trigger has been removed.
- **Negative Ztest:** Deliberately wrong assertion produced a failing Twister case and failed firmware CI in [run #39619704363](https://github.com/Titanium-Harmonics/RIDE/actions/runs/39619704363); the original assertion was restored.
- **Negative PlantUML:** A deliberately missing `!include` caused the PlantUML validation job to fail with exit code 200 in [run #39620742467](https://github.com/Titanium-Harmonics/RIDE/actions/runs/39620742467); the original diagram was restored.
- **Not yet demonstrated:** Physical ESP32-C3 test execution and manual SBOM dispatch after merge.

Negative tests above are intentionally failing historical runs, not current defects. Physical hardware testing and post-merge manual SBOM dispatch remain separate future checks.
