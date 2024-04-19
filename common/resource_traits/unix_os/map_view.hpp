#pragma once
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>

namespace nkg::resource_traits::unix_os {

    struct map_view {
        using handle_t = void*;

        static inline const handle_t invalid_value = MAP_FAILED;

        [[nodiscard]]
        static bool is_valid(const handle_t& handle) noexcept {
            return handle != invalid_value;
        }
    };

}
