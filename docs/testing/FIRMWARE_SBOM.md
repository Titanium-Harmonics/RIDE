# Firmware SBOM generation (WP-003E)

The `Firmware SBOM` GitHub Actions workflow is deliberately separate from pull-request firmware CI. It runs only through **Actions → Firmware SBOM → Run workflow** (after the workflow is present on the default branch) or on pushed release tags matching `v*`.

The workflow reuses the pinned Zephyr development container and West manifest, prepares a dedicated `build/sbom-esp32c3` build directory, enables `CONFIG_BUILD_OUTPUT_META`, builds ESP32-C3 firmware, and invokes `west spdx` using SPDX 2.3.

For each of `app`, `zephyr`, `build` and `modules-deps`, it generates the Zephyr tag-value `.spdx` file and converts it to a corresponding `.spdx.json` file using pinned `spdx-tools`. Missing or incorrect output fails the job. All reports are uploaded as one GitHub Actions artifact with 30-day retention; none are committed to Git.

**Integration validation:** WP-003E's temporary pull-request trigger successfully generated all four SPDX 2.3 JSON reports and uploaded the artifact in [GitHub Actions run #39617826449](https://github.com/Titanium-Harmonics/RIDE/actions/runs/39617826449). The temporary trigger was removed afterward. The production triggers are manual dispatch and `v*` tag pushes; manual dispatch becomes available after this workflow is merged into the default branch.
