FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG ZEPHYR_SDK_VERSION=0.17.4
ARG WEST_VERSION=1.5.0

RUN apt-get update && apt-get install -y --no-install-recommends \
    bash-completion ca-certificates ccache clang clangd clang-format cmake cppcheck \
    curl device-tree-compiler dfu-util file g++ g++-multilib gcc gcc-multilib git gperf jq less \
    libncurses-dev libusb-1.0-0 make nano ninja-build pkg-config \
    python3 python3-dev python3-pip python3-venv python3-setuptools python3-wheel \
    rsync sudo unzip usbutils udev wget xz-utils \
    && rm -rf /var/lib/apt/lists/*

RUN if ! id -u ubuntu >/dev/null 2>&1; then useradd --create-home --shell /bin/bash ubuntu; fi && \
    usermod -aG sudo ubuntu && \
    mkdir -p /home/ubuntu && \
    chown ubuntu:ubuntu /home/ubuntu && \
    echo 'ubuntu ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/ubuntu && \
    chmod 0440 /etc/sudoers.d/ubuntu

RUN python3 -m venv /home/ubuntu/venv && \
    chown -R ubuntu:ubuntu /home/ubuntu/venv
ENV PATH="/home/ubuntu/venv/bin:${PATH}"

RUN wget -q "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" -O /tmp/zephyr-sdk.tar.xz && \
    mkdir -p /opt/zephyr-sdk && \
    tar -xf /tmp/zephyr-sdk.tar.xz -C /opt/zephyr-sdk --strip-components=1 && \
    rm /tmp/zephyr-sdk.tar.xz && \
    /opt/zephyr-sdk/setup.sh -t riscv64-zephyr-elf -h -c

ENV ZEPHYR_TOOLCHAIN_VARIANT=zephyr
ENV ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk

USER ubuntu
RUN python -m pip install --no-cache-dir "west==${WEST_VERSION}" "requests>=2.31,<3"
WORKDIR /workspaces
