# navicat-keygen for linux - 如何编译？

## 1. 前提条件

1. 安装最新的 `CMake`:

   ```bash
   $ sudo apt-get install cmake
   ```

2. 安装 `fmt`、`OpenSSL` 和 `rapidjson`:

   ```bash
   $ sudo apt-get install libfmt-dev libssl-dev rapidjson-dev
   ```

## 2. 编译

1. clone 仓库:

   ```bash
   $ git clone https://github.com/FuLygon/Navicat-Linux.git
   $ cd Navicat-Linux
   ```

2. 编译:

   ```bash
   $ mkdir build
   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=Release ..
   $ cmake --build . -- -j4
   ```

   编译完后你会在 build 文件夹里看到两个可执行文件 `navicat-keygen` 和 `navicat-patcher`。
