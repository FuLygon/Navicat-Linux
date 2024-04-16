#pragma once
#include <string>
#include <vector>
#include "exception.hpp"

namespace nkg {

    struct base32_rfc4648 {
        using alphabet_index_t = size_t;

        static constexpr const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
        static constexpr const char padding_character = '=';

        class decoding_error;

        static char symbol(alphabet_index_t idx);

        static alphabet_index_t reverse_symbol(char c);

        static std::string encode(const std::vector<uint8_t>& data);

        static std::string encode(const void* data_ptr, size_t data_size);

        static std::vector<uint8_t> decode(std::string_view b32_string);
    };

    class base32_rfc4648::decoding_error : public ::nkg::exception {
        using ::nkg::exception::exception;
    };

}
