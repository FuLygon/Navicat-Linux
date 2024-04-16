# navicat-keygen for linux - How to build?

[中文版](how-to-build.zh-CN.md)

## 1. Prerequisites

1. Install latest `CMake`:

   ```bash
   $ sudo apt-get install cmake
   ```

2. Install `fmt`, `OpenSSL` and `rapidjson`:

   ```bash
   $ sudo apt-get install libfmt-dev libssl-dev rapidjson-dev
   ```

## 2. Build

1. Clone:

   ```bash
   $ git clone https://github.com/FuLygon/Navicat-Linux.git
   $ cd Navicat-Linux
   ```

2. Build:

   ```bash
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=Release ..
   $ cmake --build . -- -j4
   ```

   Then you will see two executable files, `navicat-keygen` and `navicat-patcher`, in `build` directory.
