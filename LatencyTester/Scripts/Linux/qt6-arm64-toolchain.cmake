# Auto-generated CMake toolchain for ARM64 cross-compilation
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)

set(CMAKE_SYSROOT /home/guillermogiron/ARM64-sysroot)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Ensure pkg-config looks in the sysroot
set(ENV{PKG_CONFIG_PATH} "/home/guillermogiron/ARM64-sysroot/usr/lib/aarch64-linux-gnu/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "/home/guillermogiron/ARM64-sysroot")

# GLib stores glibconfig.h in a non-standard location
include_directories(SYSTEM "/home/guillermogiron/ARM64-sysroot/usr/lib/aarch64-linux-gnu/glib-2.0/include")

# Ensure linker uses the sysroot's libc (avoids GLIBC version mismatch)
# Use lld instead of ld.bfd to avoid GLIBC version resolution issues
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld -Wl,-rpath-link,/home/guillermogiron/ARM64-sysroot/usr/lib/aarch64-linux-gnu")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld -Wl,-rpath-link,/home/guillermogiron/ARM64-sysroot/usr/lib/aarch64-linux-gnu")
