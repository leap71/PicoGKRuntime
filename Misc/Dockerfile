FROM gcc:15

# 1) Core build tools
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    wget \
    git \
    tar \
    bzip2 \
    python3 \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# 2) Install Boost and TBB (avoid problematic extras)
RUN apt-get update && apt-get install -y \
    libboost-all-dev --no-install-recommends \
    libtbb-dev \
    && rm -rf /var/lib/apt/lists/*

# 3) Install Blosc (either Blosc2 or original Blosc)
RUN git clone https://github.com/Blosc/c-blosc.git /opt/blosc \
 && cd /opt/blosc \
 && mkdir build && cd build \
 && cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. \
 && cmake --build . --target install

# 4) GLFW/ImGui/OpenGL/Wayland/X11 support
RUN apt-get update && apt-get install -y \
    libwayland-dev \
    libxkbcommon-dev \
    wayland-protocols \
    extra-cmake-modules \
    xorg-dev \
    mesa-common-dev \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
