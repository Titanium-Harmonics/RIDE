APP := app
BOARD ?= esp32c3_devkitm/esp32c3
BUILD_DIR ?= build/$(subst /,_,$(BOARD))

.DEFAULT_GOAL := help
.PHONY: help setup update doctor build pristine flash fw-package fw-download clean menuconfig guiconfig format format-check lint test

help:
	@echo "R.I.D.E. Zephyr development commands"
	@echo "  make setup         Fetch pinned Zephyr workspace, Python dependencies and ESP HAL blobs"
	@echo "  make doctor        Print tool versions and verify workspace"
	@echo "  make build         Build BOARD=$(BOARD)"
	@echo "  make pristine      Clean rebuild"
	@echo "  make flash         Flash BOARD=$(BOARD) (requires USB device)"
	@echo "  make fw-package    Bundle ESP32-C3 firmware and local flashing scripts"
	@echo "  make fw-download   Bundle and offer a private Codespaces download link"
	@echo "  make test          Run host-based Ztest suites with Twister (native_sim)"
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
	west build -b $(BOARD) $(APP) -d $(BUILD_DIR)

pristine:
	west build -p always -b $(BOARD) $(APP) -d $(BUILD_DIR)

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
	west twister -T tests/ci -p native_sim --inline-logs --outdir build/twister

format:
	clang-format -i $(shell git ls-files 'app/*.c' 'app/*.h' 'app/**/*.c' 'app/**/*.h' 'tests/**/*.c' 'tests/**/*.h')

format-check:
	clang-format --dry-run --Werror $(shell git ls-files 'app/*.c' 'app/*.h' 'app/**/*.c' 'app/**/*.h' 'tests/**/*.c' 'tests/**/*.h')

lint:
	cppcheck --error-exitcode=1 --enable=warning,style,performance --std=c99 app/src