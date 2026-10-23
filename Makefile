APP := app
BOARD ?= esp32c3_devkitm/esp32c3
BUILD_DIR ?= build/$(subst /,_,$(BOARD))
OVERLAY_FILE ?= boards/esp32c3_devkitm.overlay

.DEFAULT_GOAL := help
.PHONY: help setup update doctor build pristine flash fw-package fw-download clean menuconfig guiconfig format format-check lint test test-report

help:
	@echo "R.I.D.E. Zephyr development commands"
	@echo "  make setup         Fetch pinned Zephyr workspace, Python dependencies and ESP HAL blobs"
	@echo "  make doctor        Print tool versions and verify workspace"
	@echo "  make build         Build BOARD=$(BOARD)"
	@echo "  make pristine      Clean rebuild"
	@echo "  make flash         Flash BOARD=$(BOARD) (requires USB device)"
	@echo "  make fw-package    Bundle ESP32-C3 firmware and local flashing scripts"
	@echo "  make fw-download   Bundle and offer a private Codespaces download link"
	@echo "  make test          Run native sanitizer tests and Twister (native_sim)"
	@echo "  make test-report   Run tests and generate build/reports/index.html"
	@echo "  make format-check  Check tracked C source formatting"
	@echo "  make lint          Run cppcheck on app source"

setup:
	@if [ ! -d .west ]; then west init -l $(APP); else echo "Using existing .west workspace"; fi
	west update
	west packages pip --install
	west blobs fetch hal_espressif
	west zephyr-export

update:
	west update
	west packages pip --install
	west blobs fetch hal_espressif

doctor:
	@python --version
	@west --version
	@cmake --version | head -n 1
	@west topdir
	@west list zephyr -f "{revision}"

build:
	west build -b $(BOARD) $(APP) -d $(BUILD_DIR) -- -DDTC_OVERLAY_FILE=$(OVERLAY_FILE)

pristine:
	west build -p always -b $(BOARD) $(APP) -d $(BUILD_DIR) -- -DDTC_OVERLAY_FILE=$(OVERLAY_FILE)

flash:
	west flash -d $(BUILD_DIR)

fw-package:
	python scripts/flash/package.py --build-dir $(BUILD_DIR)

fw-download: fw-package
	python scripts/flash/download.py dist/RIDE-ESP32C3.zip

clean:
	rm -rf build

menuconfig:
	west build -t menuconfig -d $(BUILD_DIR)

guiconfig:
	west build -t guiconfig -d $(BUILD_DIR)

test:
	python scripts/testing/run_host_tests.py
	west twister -T tests/ci -p native_sim --inline-logs --outdir build/twister

test-report:
	@$(MAKE) test; status=$?; \
	python scripts/testing/html_report.py --xml build/twister/twister_report.xml --host-xml build/tests/host_report.xml --output build/reports/index.html || exit $$?; \
	exit $$status

format:
	clang-format -i $(shell git ls-files 'app/*.c' 'app/*.h' 'app/**/*.c' 'app/**/*.h' 'tests/**/*.c' 'tests/**/*.h')

format-check:
	clang-format --dry-run --Werror $(shell git ls-files 'app/*.c' 'app/*.h' 'app/**/*.c' 'app/**/*.h' 'tests/**/*.c' 'tests/**/*.h')

lint:
	cppcheck --error-exitcode=1 --enable=warning,style,performance --std=c99 app/src