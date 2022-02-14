# navicat-keygen for linux - How to build?

[中文版](how-to-build.zh-CN.md)

## 1. Prerequisites

1. Install latest `CMake`:

   ```bash
   $ sudo apt-get install cmake
   ```

2. Install `fmt`, `OpenSSL`, `rapidjson` and `cxxopts`:

   ```bash
   $ sudo apt-get install libfmt-dev libssl-dev rapidjson-dev libcxxopts-dev
   ```

## 2. Build

1. Clone:

   ```bash
   $ git clone -b linux --single-branch https://notabug.org/doublesine/navicat-keygen.git
   $ cd navicat-keygen
   ```

2. Build:

   ```bash
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=Release ..
   $ cmake --build . -- -j4
   ```

   Then you will see two executable files, `navicat-keygen` and `navicat-patcher`, in `build` directory.
