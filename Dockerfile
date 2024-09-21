FROM fedora:40

RUN --mount=type=cache,target=/var/cache/dnf dnf install -y \
        dev86 \
        gcc-c++-x86_64-linux-gnu \
        gcc-x86_64-linux-gnu \
        binutils-x86_64-linux-gnu \
        qemu-system-x86 \
        make \
        g++ \
        gcc \
        fish \
        git

WORKDIR /build
