#pragma once
#include <string>
#include <vector>
#include "exception.hpp"

namespace nkg {

    struct base64_rfc4648 {
        class backend_error;

        static std::string encode(const std::vector<std::uint8_t>& data);

        static std::vector<uint8_t> decode(std::string_view str_b64);
    };

    class base64_rfc4648::backend_error : public ::nkg::exception {
        using ::nkg::exception::exception;
    };

}
