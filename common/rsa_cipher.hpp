#pragma once
#include <string>
#include <optional>

#include <openssl/err.h>
#include <openssl/rsa.h>

#include "resource_wrapper.hpp"
#include "resource_traits/openssl/rsa.hpp"

#include "exception.hpp"

#define NKG_CURRENT_SOURCE_FILE() u8".\\common\\rsa_cipher.hpp"
#define NKG_CURRENT_SOURCE_LINE() __LINE__

namespace nkg {

    class rsa_cipher {
    public:
        class no_key_assigned_error : public ::nkg::exception {
        public:
            no_key_assigned_error(std::string_view file, int line, std::string_view message) noexcept :
                ::nkg::exception(file, line, message) {}
        };

        class backend_error : public ::nkg::exception {
        public:
            using error_code_t = decltype(ERR_get_error());

        private:
            std::optional<error_code_t> m_error_code;
            std::string m_error_string;

        public:
            backend_error(std::string_view file, int line, std::string_view message) noexcept :
                ::nkg::exception(file, line, message) {}

            backend_error(std::string_view file, int line, error_code_t openssl_errno, std::string_view message) noexcept :
                ::nkg::exception(file, line, message), m_error_code(openssl_errno) {}

            [[nodiscard]]
            virtual bool error_code_exists() const noexcept override {
                return m_error_code.has_value();
            }

            [[nodiscard]]
            virtual intptr_t error_code() const noexcept override {
                if (error_code_exists()) { return m_error_code.value(); } else { trap_then_terminate(); }
            }

            [[nodiscard]]
            virtual const std::string& error_string() const noexcept override {
                if (error_code_exists()) { return m_error_string; } else { trap_then_terminate(); }
            }
        };

    private:
        resource_wrapper<resource_traits::openssl::rsa> m_rsa;

        [[nodiscard]]
        static RSA* _read_private_key_from_bio(BIO* p_bio);

        [[nodiscard]]
        static RSA* _read_public_key_pem_from_bio(BIO* p_bio);

        [[nodiscard]]
        static RSA* _read_public_key_pkcs1_from_bio(BIO* p_bio);

        static void _write_private_key_to_bio(RSA* p_rsa, BIO* p_bio);

        static void _write_public_key_pem_to_bio(RSA* p_rsa, BIO* p_bio);

        static void _write_public_key_pkcs1_to_bio(RSA* p_rsa, BIO* p_bio);

    public:

        rsa_cipher();

        [[nodiscard]]
        size_t bits() const;

        void generate_key(int bits, unsigned int e = RSA_F4);

        void export_private_key_file(std::string_view file_path) const;

        void export_public_key_file_pem(std::string_view file_path) const;

        void export_public_key_file_pkcs1(std::string_view file_path) const;

        void import_private_key_file(std::string_view file_path);

        void import_public_key_file_pem(std::string_view file_path);

        void import_public_key_file_pkcs1(std::string_view file_path);

        [[nodiscard]]
        std::string export_private_key_string() const;

        [[nodiscard]]
        std::string export_public_key_string_pem() const;

        [[nodiscard]]
        std::string export_public_key_string_pkcs1() const;

        void import_private_key_string(std::string_view key_string);

        void import_public_key_string_pem(std::string_view key_string);

        void import_public_key_string_pkcs1(std::string_view key_string);

        size_t public_encrypt(const void* plaintext, size_t plaintext_size, void* ciphertext, int padding) const;

        size_t private_encrypt(const void* plaintext, size_t plaintext_size, void* ciphertext, int padding) const;

        size_t public_decrypt(const void* ciphertext, size_t ciphertext_size, void* plaintext, int padding) const;

        size_t private_decrypt(const void* ciphertext, size_t ciphertext_size, void* plaintext, int padding) const;
    };

}

#undef NKG_CURRENT_SOURCE_FILE
#undef NKG_CURRENT_SOURCE_LINE
