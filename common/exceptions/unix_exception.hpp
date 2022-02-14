#pragma once
#include <errno.h>
#include <string.h>
#include "../exception.hpp"

namespace nkg::exceptions {

    class unix_exception : public ::nkg::exception {
    public:
        using error_code_t = decltype(errno);

    private:
        error_code_t m_error_code;
        std::string m_error_string;

    public:
        unix_exception(std::string_view file, int line, error_code_t unix_errno, std::string_view message) noexcept :
            ::nkg::exception(file, line, message), m_error_code(unix_errno), m_error_string(strerror(unix_errno)) {}

        [[nodiscard]]
        virtual bool error_code_exists() const noexcept override {
            return true;
        }

        [[nodiscard]]
        virtual intptr_t error_code() const noexcept override {
            return m_error_code;
        }

        [[nodiscard]]
        virtual const std::string& error_string() const noexcept override {
            return m_error_string;
        }
    };

}
