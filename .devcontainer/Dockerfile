FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends  \
    libopenblas-dev

WORKDIR /tmp
RUN wget -q https://github.com/facebookresearch/faiss/archive/refs/tags/v1.10.0.tar.gz \
    && tar -xzf v1.10.0.tar.gz \
    && cd faiss-1.10.0 \
    && cmake -B build . -DFAISS_ENABLE_GPU=OFF -DFAISS_ENABLE_PYTHON=OFF -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release \
    && make -C build -j$(nproc) faiss \
    && make -C build install \
    && cd /tmp \
    && rm v1.10.0.tar.gz \
    && rm -rf faiss-1.10.0

#WORKDIR /tmp
#RUN
