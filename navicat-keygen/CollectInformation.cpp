#include <iostream>
#include "navicat_serial_generator.hpp"
#include "exceptions/operation_canceled_exception.hpp"

#define NKG_CURRENT_SOURCE_FILE() u8".\\navicat-keygen\\CollectInformation.cpp"
#define NKG_CURRENT_SOURCE_LINE() __LINE__

namespace nkg {

    [[nodiscard]]
    static int read_int(int min_val, int max_val, std::string_view prompt, std::string_view error_msg) {
        int val;

        for (std::string s;;) {
            std::cout << prompt;
            if (!std::getline(std::cin, s)) {
                throw exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Operation is canceled by user.");
            }

            if (s.empty())
                continue;

            try {
                val = std::stoi(s, nullptr, 0);
                if (min_val <= val && val <= max_val) {
                    return val;
                } else {
                    throw std::invalid_argument("Out of range.");
                }
            } catch (std::invalid_argument&) {
                std::cout << error_msg << std::endl;
            }
        }
    }

    [[nodiscard]]
    static int read_int(int min_val, int max_val, int default_val, std::string_view prompt, std::string_view error_msg) {
        int val;

        for (std::string s;;) {
            std::cout << prompt;
            if (!std::getline(std::cin, s)) {
                throw exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"Operation is canceled by user.");
            }

            if (s.empty()) {
                return default_val;
            }

            try {
                val = std::stoi(s, nullptr, 0);
                if (min_val <= val && val <= max_val) {
                    return val;
                } else {
                    throw std::invalid_argument("Out of range.");
                }
            } catch (std::invalid_argument&) {
                std::cout << error_msg << std::endl;
            }
        }
    }

    [[nodiscard]]
    navicat_serial_generator CollectInformationNormal() {
        navicat_serial_generator sn_generator;

        std::cout << "[*] Select Navicat product:"  << std::endl;
        std::cout << " 0. DataModeler"              << std::endl;
        std::cout << " 1. Premium"                  << std::endl;
        std::cout << " 2. MySQL"                    << std::endl;
        std::cout << " 3. PostgreSQL"               << std::endl;
        std::cout << " 4. Oracle"                   << std::endl;
        std::cout << " 5. SQLServer"                << std::endl;
        std::cout << " 6. SQLite"                   << std::endl;
        std::cout << " 7. MariaDB"                  << std::endl;
        std::cout << " 8. MongoDB"                  << std::endl;
        std::cout << " 9. ReportViewer"             << std::endl;
        std::cout << " 10. ChartsCreator"           << std::endl;
        std::cout << " 11. ChartsViewer"            << std::endl;
        std::cout << std::endl;
        sn_generator.set_software_type(static_cast<navicat_software_type>(read_int(0, 11, "(Input index)> ", "Invalid index.")));

        std::cout << std::endl;
        std::cout << "[*] Select product language:" << std::endl;
        std::cout << " 0. English"                  << std::endl;
        std::cout << " 1. Simplified Chinese"       << std::endl;
        std::cout << " 2. Traditional Chinese"      << std::endl;
        std::cout << " 3. Japanese"                 << std::endl;
        std::cout << " 4. Polish"                   << std::endl;
        std::cout << " 5. Spanish"                  << std::endl;
        std::cout << " 6. French"                   << std::endl;
        std::cout << " 7. German"                   << std::endl;
        std::cout << " 8. Korean"                   << std::endl;
        std::cout << " 9. Russian"                  << std::endl;
        std::cout << " 10. Portuguese"              << std::endl;
        std::cout << std::endl;
        sn_generator.set_software_language(static_cast<navicat_software_language>(read_int(0, 10, "(Input index)> ", "Invalid index.")));

        std::cout << std::endl;
        std::cout << "[*] Input major version number:" << std::endl;
        sn_generator.set_software_version(read_int(1, 16, 16, "(range: 1 ~ 16, default: 16)> ", "Invalid number."));

        std::cout << std::endl;
        return sn_generator;
    }

    [[nodiscard]]
    navicat_serial_generator CollectInformationAdvanced() {
        navicat_serial_generator sn_generator;

        std::cout << "[*] Navicat Product Signature:" << std::endl;
        sn_generator.set_software_type(static_cast<uint8_t>(read_int(0x00, 0xff, "(range: 0x00 ~ 0xFF)> ", "Invalid number.")));

        std::cout << std::endl;
        std::cout << "[*] Navicat Language Signature 0:" << std::endl;
        auto s1 = static_cast<uint8_t>(read_int(0x00, 0xff, "(range: 0x00 ~ 0xFF)> ", "Invalid number."));

        std::cout << std::endl;
        std::cout << "[*] Navicat Language Signature 1:" << std::endl;
        auto s2 = static_cast<uint8_t>(read_int(0x00, 0xff, "(range: 0x00 ~ 0xFF)> ", "Invalid number."));
        
        sn_generator.set_software_language(s1, s2);

        std::cout << std::endl;
        std::cout << "[*] Input major version number:" << std::endl;
        sn_generator.set_software_version(read_int(1, 16, 16, "(range: 1 ~ 16, default: 16)> ", "Invalid number."));

        std::cout << std::endl;
        return sn_generator;
    }
}

#undef NKG_CURRENT_SOURCE_FILE
#undef NKG_CURRENT_SOURCE_LINE
