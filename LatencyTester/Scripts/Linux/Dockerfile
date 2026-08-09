# Dockerfile - LatencyTester ARM64 test environment with VNC
# Emulates Raspberry Pi runtime for testing the application graphically.
#
# Build:  docker build --platform linux/arm64 -t latencytester-arm64 .
# Run:    docker run --platform linux/arm64 -p 5900:5900 -it latencytester-arm64

FROM arm64v8/debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive
ENV QT_QPA_PLATFORM=vnc
ENV DISPLAY=:0

# Install Qt 6 runtime libraries + VNC + tools + dev headers for cross-compilation
RUN apt-get update && apt-get install -y --no-install-recommends \
    libqt6widgets6 \
    libqt6concurrent6 \
    libqt6printsupport6 \
    libqt6gui6 \
    libqt6core6 \
    libqt6network6 \
    libqt6opengl6 \
    libqt6qml6 \
    libqt6quick6 \
    qt6-qpa-plugins \
    qt6-virtualkeyboard-plugin \
    libhunspell-dev \
    libudev-dev \
    libbrotli-dev \
    libdbus-1-dev \
    libdbus-1-3 \
    libfontconfig-dev \
    libfreetype-dev \
    libharfbuzz-dev \
    libicu-dev \
    libinput-dev \
    libjpeg-dev \
    libpng-dev \
    libssl-dev \
    libxkbcommon-dev \
    libmtdev-dev \
    libsystemd-dev \
    libatspi2.0-dev \
    libglib2.0-dev \
    libdrm-dev \
    libgbm-dev \
    zlib1g-dev \
    libgl1-mesa-dri \
    libgl-dev \
    libegl-dev \
    libgles-dev \
    libgles2 \
    libegl1 \
    x11vnc \
    xvfb \
    valgrind \
    procps \
    && rm -rf /var/lib/apt/lists/*

# Create working directory for the application
WORKDIR /app

# Copy the start script
COPY docker-entrypoint.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/docker-entrypoint.sh

# Expose VNC port
EXPOSE 5900

# Default entrypoint starts Xvfb + VNC server
ENTRYPOINT ["docker-entrypoint.sh"]
CMD ["bash"]
