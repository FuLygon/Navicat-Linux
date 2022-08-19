cmake_minimum_required(VERSION 3.18)
project(navicat-keygen)

set(CMAKE_CXX_STANDARD 17)

find_package(fmt REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(OpenSSL REQUIRED)

include(FetchContent)
set(KEYSTONE_BUILD_STATIC_RUNTIME ON CACHE BOOL "" FORCE)
set(BUILD_LIBS_ONLY ON CACHE BOOL "" FORCE)
set(UNICORN_ARCH "x86" CACHE STRING "" FORCE)
set(UNICORN_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(UNICORN_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    keystone
        GIT_REPOSITORY "https://github.com/keystone-engine/keystone.git"
        GIT_TAG "1475885daa7e566c064ae9754706e1a0ba24be3b"
)
FetchContent_Declare(
    unicorn
        URL "https://github.com/unicorn-engine/unicorn/archive/refs/tags/2.0.0.tar.gz"
        URL_HASH "SHA256=67b445c760e2bbac663e8c8bc410e43311c7fc92df4dfa8d90e06a021d07f634"
)
FetchContent_MakeAvailable(keystone unicorn)

set(
    NKG_COMMON_SOURCE
        ./common/exception.hpp
        ./common/exceptions/index_exception.hpp
        ./common/exceptions/key_exception.hpp
        ./common/exceptions/operation_canceled_exception.hpp
        ./common/exceptions/unix_exception.hpp
        ./common/resource_wrapper.hpp
        ./common/resource_traits/cxx_object_traits.hpp
        ./common/resource_traits/cxx_dynamic_array_traits.hpp
        ./common/resource_traits/keystone/keystone_alloc.hpp
        ./common/resource_traits/keystone/keystone_handle.hpp
        ./common/resource_traits/openssl/bignum.hpp
        ./common/resource_traits/openssl/bio.hpp
        ./common/resource_traits/openssl/bio_chain.hpp
        ./common/resource_traits/openssl/rsa.hpp
        ./common/resource_traits/openssl/evp_pkey.hpp
        ./common/resource_traits/openssl/evp_pkey_ctx.hpp
        ./common/resource_traits/openssl/evp_cipher_ctx.hpp
        ./common/resource_traits/openssl/encoder_ctx.hpp
        ./common/resource_traits/openssl/decoder_ctx.hpp
        ./common/resource_traits/unicorn/unicorn_handle.hpp
        ./common/resource_traits/unicorn/unicorn_alloc.hpp
        ./common/resource_traits/unix_os/file_descriptor.hpp
        ./common/resource_traits/unix_os/map_view.hpp
        ./common/rsa_cipher.hpp
        ./common/rsa_cipher.cpp
)

set(
    NKG_KEYGEN_SOURCE
        ./navicat-keygen/base32_rfc4648.hpp
        ./navicat-keygen/base32_rfc4648.cpp
        ./navicat-keygen/base64_rfc4648.hpp
        ./navicat-keygen/base64_rfc4648.cpp
        ./navicat-keygen/navicat_serial_generator.hpp
        ./navicat-keygen/navicat_serial_generator.cpp
        ./navicat-keygen/CollectInformation.cpp
        ./navicat-keygen/GenerateLicense.cpp
        ./navicat-keygen/main.cpp
)

set(
    NKG_PATCHER_SOURCE
        ./navicat-patcher/amd64_emulator.hpp
        ./navicat-patcher/amd64_emulator.cpp
        ./navicat-patcher/keystone_assembler.hpp
        ./navicat-patcher/keystone_assembler.cpp
        ./navicat-patcher/elf64_interpreter.hpp
        ./navicat-patcher/elf64_interpreter.cpp
        ./navicat-patcher/patch_solution.hpp
        ./navicat-patcher/patch_solution_since.hpp
        ./navicat-patcher/patch_solution_since_16.0.7.0.hpp
        ./navicat-patcher/patch_solution_since_16.0.7.0.cpp
        ./navicat-patcher/memory_utility.hpp
        ./navicat-patcher/main.cpp
)

add_executable(navicat-keygen ${NKG_COMMON_SOURCE} ${NKG_KEYGEN_SOURCE})
target_include_directories(navicat-keygen PRIVATE ./common ${RAPIDJSON_INCLUDE_DIRS})
target_link_libraries(navicat-keygen fmt::fmt OpenSSL::Crypto)

add_executable(navicat-patcher ${NKG_COMMON_SOURCE} ${NKG_PATCHER_SOURCE})
target_include_directories(navicat-patcher PRIVATE ./common ${keystone_SOURCE_DIR}/include)
target_link_libraries(navicat-patcher fmt::fmt OpenSSL::Crypto keystone unicorn pthread)
