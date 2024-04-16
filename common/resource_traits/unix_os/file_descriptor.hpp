#pragma once
#include <unistd.h>

namespace nkg::resource_traits::unix_os {

    struct file_descriptor {
        using handle_t = int;

        static constexpr handle_t invalid_value = -1;

        [[nodiscard]]
        static bool is_valid(const handle_t& handle) noexcept {
            return handle != invalid_value;
        }

        static void release(const handle_t& handle) {
            close(handle);
        }
    };

}
