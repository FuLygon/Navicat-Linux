#include <iostream>
#include <ctime>
#include <fmt/format.h>
#include <fcntl.h>

#include "exceptions/operation_canceled_exception.hpp"
#include "exceptions/unix_exception.hpp"

#include "resource_wrapper.hpp"
#include "resource_traits/unix_os/file_descriptor.hpp"

#include "rsa_cipher.hpp"
#include "navicat_serial_generator.hpp"
#include "base64_rfc4648.hpp"

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#define NKG_CURRENT_SOURCE_FILE() u8".\\navicat-keygen\\GenerateLicense.cpp"
#define NKG_CURRENT_SOURCE_LINE() __LINE__

namespace nkg {

    void GenerateLicenseText(const rsa_cipher& cipher, const navicat_serial_generator& sn_generator) {
        std::string u8_username;
        std::string u8_organization;

        std::string b64_request_code;
        std::vector<uint8_t> request_code;
        std::string u8_request_info;
        std::string u8_response_info;
        std::vector<uint8_t> response_code;
        std::string b64_response_code;

        std::cout << "[*] Your name: ";
        if (!std::getline(std::cin, u8_username)) {
            throw exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Operation is canceled by user.");
        }

        std::cout << "[*] Your organization: ";
        if (!std::getline(std::cin, u8_organization)) {
            throw exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Operation is canceled by user.");
        }

        std::cout << std::endl;

        std::cout << "[*] Input request code in Base64: (Double press ENTER to end)" << std::endl;
        while (true) {
            std::string temp;
            if (!std::getline(std::cin, temp)) {
                throw exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Operation is canceled by user.");
            }

            if (temp.empty()) {
                break;
            }

            b64_request_code.append(temp);
        }

        request_code = base64_rfc4648::decode(b64_request_code);
        if (request_code.size() != 256) {
            throw ::nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Broken request code. %zu", request_code.size()));
        }

        u8_request_info.resize((cipher.bits() + 7) / 8);
        u8_request_info.resize(cipher.private_decrypt(request_code.data(), request_code.size(), u8_request_info.data(), RSA_PKCS1_PADDING));

        std::cout << "[*] Request Info:" << std::endl;
        std::cout << u8_request_info << std::endl;
        std::cout << std::endl;

        rapidjson::Document json;
        rapidjson::Value N_Key;
        rapidjson::Value N_Value;
        rapidjson::Value O_Key;
        rapidjson::Value O_Value;
        rapidjson::Value T_Key;
        rapidjson::Value T_Value;
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        //
        // Begin to parse
        //
        json.Parse(u8_request_info.c_str());
        //
        // Remove "Platform" info
        //
        json.RemoveMember("P");
        //
        // Set "Name" info
        //
        N_Key.SetString("N", 1);
        N_Value.SetString(u8_username.c_str(), static_cast<rapidjson::SizeType>(u8_username.length()));
        //
        // Set "Organization" info
        //
        O_Key.SetString("O", 1);
        O_Value.SetString(u8_organization.c_str(), static_cast<rapidjson::SizeType>(u8_organization.length()));
        //
        // Set "Time" info
        //
        T_Key.SetString("T", 1);
        T_Value.SetUint(static_cast<unsigned int>(std::time(nullptr)));
        //
        // Add "Name", "Organization" and "Time"
        //
        json.AddMember(N_Key, N_Value, json.GetAllocator());
        json.AddMember(O_Key, O_Value, json.GetAllocator());
        json.AddMember(T_Key, T_Value, json.GetAllocator());

        json.Accept(writer);
        if (buffer.GetSize() > 240) {
            throw ::nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Response Info is too long.");
        }

        u8_response_info.assign(buffer.GetString(), buffer.GetSize());

        std::cout << "[*] Response Info:" << std::endl;
        std::cout << u8_response_info << std::endl;
        std::cout << std::endl;

        response_code.resize((cipher.bits() + 7) / 8);
        response_code.resize(cipher.private_encrypt(u8_response_info.data(), u8_response_info.size(), response_code.data(), RSA_PKCS1_PADDING));

        b64_response_code = base64_rfc4648::encode(response_code);

        std::cout << "[*] Activation Code:" << std::endl;
        std::cout << b64_response_code << std::endl;
        std::cout << std::endl;
    }

    void GenerateLicenseBinary(const rsa_cipher& cipher, const navicat_serial_generator& sn_generator) {
        std::string u8_serial_number = sn_generator.serial_number();

        std::string u8_username;
        std::string u8_organization;

        std::string u8_response_info;
        std::vector<uint8_t> response_code;

        std::cout << "[*] Your name: ";
        if (!std::getline(std::cin, u8_username)) {
            throw exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Operation is canceled by user.");
        }

        std::cout << "[*] Your organization: ";
        if (!std::getline(std::cin, u8_organization)) {
            throw exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Operation is canceled by user.");
        }

        std::cout << std::endl;

        rapidjson::Document json;
        rapidjson::Value N_Key;
        rapidjson::Value N_Value;
        rapidjson::Value O_Key;
        rapidjson::Value O_Value;
        rapidjson::Value T_Key;
        rapidjson::Value T_Value;
        rapidjson::Value K_Key;
        rapidjson::Value K_Value;
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        json.Parse("{}");
        K_Key.SetString("K", 1);
        K_Value.SetString(u8_serial_number.c_str(), static_cast<rapidjson::SizeType>(u8_serial_number.length()));
        N_Key.SetString("N", 1);
        N_Value.SetString(u8_username.c_str(), static_cast<rapidjson::SizeType>(u8_username.length()));
        O_Key.SetString("O", 1);
        O_Value.SetString(u8_organization.c_str(), static_cast<rapidjson::SizeType>(u8_organization.length()));
        T_Key.SetString("T", 1);
        T_Value.SetUint(static_cast<unsigned int>(std::time(nullptr)));

        json.AddMember(K_Key, K_Value, json.GetAllocator());
        json.AddMember(N_Key, N_Value, json.GetAllocator());
        json.AddMember(O_Key, O_Value, json.GetAllocator());
        json.AddMember(T_Key, T_Value, json.GetAllocator());

        json.Accept(writer);
        if (buffer.GetSize() > 240) {
            throw ::nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Response Info is too long.");
        }

        u8_response_info.assign(buffer.GetString(), buffer.GetSize());

        std::cout << "[*] Response Info:" << std::endl;
        std::cout << u8_response_info << std::endl;
        std::cout << std::endl;

        response_code.resize((cipher.bits() + 7) / 8);
        response_code.resize(cipher.private_encrypt(u8_response_info.data(), u8_response_info.size(), response_code.data(), RSA_PKCS1_PADDING));

        resource_wrapper license_file{ resource_traits::unix_os::file_descriptor{}, open("license_file", O_WRONLY | O_CREAT) };
        if (!license_file.is_valid()) {
            throw exceptions::unix_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), errno, u8"open failed.");
        }

        if (write(license_file.get(), response_code.data(), response_code.size()) < 0) {
            throw exceptions::unix_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), errno, u8"write failed.");
        }

        std::cout << "[+] license_file has been generated." << std::endl;
    }
}

#undef NKG_CURRENT_SOURCE_FILE
#undef NKG_CURRENT_SOURCE_LINE
