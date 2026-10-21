# Firmware SBOM generation (WP-003E)

The `Firmware SBOM` GitHub Actions workflow is deliberately separate from pull-request firmware CI. It runs only through **Actions → Firmware SBOM → Run workflow** (after the workflow is present on the default branch) or on pushed release tags matching `v*`.

The workflow reuses the pinned Zephyr development container and West manifest, prepares a dedicated `build/sbom-esp32c3` build directory, enables `CONFIG_BUILD_OUTPUT_META`, builds ESP32-C3 firmware, and invokes `west spdx` using SPDX 2.3.

For each of `app`, `zephyr`, `build` and `modules-deps`, it generates the Zephyr tag-value `.spdx` file and converts it to a corresponding `.spdx.json` file using pinned `spdx-tools`. Missing or incorrect output fails the job. All reports are uploaded as one GitHub Actions artifact with 30-day retention; none are committed to Git.

**Validation:** The workflow's YAML and shell syntax were checked locally before commit. Actual `west spdx` generation and conversion require the Zephyr environment and remain to be checked in a GitHub Actions integration run. A manual dispatch cannot target a new workflow until GitHub recognizes it on the default branch.
