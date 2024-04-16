#pragma once
#include <exception>
#include <string>
#include <vector>
#include <utility>

namespace nkg {

    class exception : public std::exception {
    private:
        int m_source_line;
        std::string m_source_file;
        std::string m_custom_message;
        std::vector<std::string> m_hints;

    public:
        [[noreturn]]
        static void trap_then_terminate() {
#if _MSC_VER
            __debugbreak();
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
            __builtin_trap();

#else
#error "exception.hpp: unknown compiler is detected."
#endif
            std::terminate();
        }

        exception(std::string_view file, int line, std::string_view message) noexcept :
            std::exception(), m_source_line(line), m_source_file(file), m_custom_message(message) {}

        exception(const exception&) noexcept = default;
        exception(exception&&) noexcept = default;

        exception& operator=(const exception&) noexcept = default;
        exception& operator=(exception&&) noexcept = default;

        [[nodiscard]]
        int source_line() const noexcept {
            return m_source_line;
        }

        [[nodiscard]]
        const std::string& source_file() const noexcept {
            return m_source_file;
        }

        [[nodiscard]]
        const std::string& custom_message() const noexcept {
            return m_custom_message;
        }

        exception&& push_hint(std::string_view hint) noexcept {
            m_hints.emplace_back(hint);
            return std::move(*this);
        }

        exception&& pop_hint() noexcept {
            m_hints.pop_back();
            return std::move(*this);
        }

        const std::vector<std::string>& hints() const noexcept {
            return m_hints;
        }

        virtual const char* what() const noexcept override {
            return m_custom_message.c_str();
        }

        [[nodiscard]]
        virtual bool error_code_exists() const noexcept {
            return false;
        }

        [[nodiscard]]
        virtual intptr_t error_code() const noexcept {
            trap_then_terminate();
        }

        [[nodiscard]]
        virtual const std::string& error_string() const noexcept {
            trap_then_terminate();
        }

        virtual ~exception() override = default;
    };

}
