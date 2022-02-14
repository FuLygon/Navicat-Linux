#include <stdio.h>
#include <string.h>
#include <functional>

#include "exception.hpp"
#include "exceptions/operation_canceled_exception.hpp"

#include "base64_rfc4648.hpp"
#include "navicat_serial_generator.hpp"
#include "rsa_cipher.hpp"

#define NKG_CURRENT_SOURCE_FILE() u8".\\navicat-keygen\\wmain.cpp"
#define NKG_CURRENT_SOURCE_LINE() __LINE__

namespace nkg {
    using fnCollectInformation = std::function<navicat_serial_generator()>;
    using fnGenerateLicense = std::function<void(const rsa_cipher& cipher, const navicat_serial_generator& generator)>;

    navicat_serial_generator CollectInformationNormal();
    navicat_serial_generator CollectInformationAdvanced();
    void GenerateLicenseText(const rsa_cipher& cipher, const navicat_serial_generator& sn_generator);
    void GenerateLicenseBinary(const rsa_cipher& cipher, const navicat_serial_generator& sn_generator);
}

void welcome() {
    puts("***************************************************");
    puts("*       navicat-keygen by @DoubleLabyrinth        *");
    puts("*                version: 16.0.7.0                *");
    puts("***************************************************");
    puts("");
}

void help() {
    puts("Usage:");
    puts("    navicat-keygen.exe <-bin|-text> [-adv] <RSA-2048 Private Key File>");
    puts("");
    puts("    <-bin|-text>       Specify \"-bin\" to generate \"license_file\" used by Navicat 11.");
    puts("                       Specify \"-text\" to generate base64-encoded activation code.");
    puts("                       This parameter must be specified.");
    puts("");
    puts("    [-adv]             Enable advance mode.");
    puts("                       This parameter is optional.");
    puts("");
    puts("    <RSA-2048 Private Key File>    A path to an RSA-2048 private key file.");
    puts("                                   This parameter must be specified.");
    puts("");
    puts("Example:");
    puts("    navicat-keygen.exe -text .\\RegPrivateKey.pem");
}

int main(int argc, char* argv[]) {
    welcome();

    if (argc == 3 || argc == 4) {
        nkg::fnCollectInformation lpfnCollectInformation;
        nkg::fnGenerateLicense lpfnGenerateLicense;

        if (strcmp(argv[1], "-bin") == 0) {
            lpfnGenerateLicense = nkg::GenerateLicenseBinary;
        } else if (strcmp(argv[1], "-text") == 0) {
            lpfnGenerateLicense = nkg::GenerateLicenseText;
        } else {
            help();
            return -1;
        }

        if (argc == 4) {
            if (strcmp(argv[2], "-adv") == 0) {
                lpfnCollectInformation = nkg::CollectInformationAdvanced;
            } else {
                help();
                return -1;
            }
        } else {
            lpfnCollectInformation = nkg::CollectInformationNormal;
        }

        try {
            nkg::rsa_cipher cipher;

            cipher.import_private_key_file(argv[argc - 1]);
            if (cipher.bits() != 2048) {
                throw nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), u8"RSA key length mismatches.")
                    .push_hint(u8"You must provide an RSA key whose modulus length is 2048 bits.");
            }

            auto sn_generator = lpfnCollectInformation();
            sn_generator.generate();

            puts("[*] Serial number:");
            puts(sn_generator.serial_number_formatted().c_str());
            puts("");

            lpfnGenerateLicense(cipher, sn_generator);

            return 0;
        } catch (nkg::exceptions::operation_canceled_exception&) {
            return -1;
        } catch (nkg::exception& e) {
            printf("[-] %s:%d ->\n", e.source_file().c_str(), e.source_line());
            printf("    %s\n", e.custom_message().c_str());

            if (e.error_code_exists()) {
                printf("    %s (0x%zx)\n", e.error_string().c_str(), e.error_code());
            }

            for (auto& hint : e.hints()) {
                printf("    Hints: %s\n", hint.c_str());
            }

            return -1;
        } catch (std::exception& e) {
            printf("[-] %s\n", e.what());
            return -1;
        }
    } else {
        help();
        return -1;
    }
}

#undef NKG_CURRENT_SOURCE_FILE
#undef NKG_CURRENT_SOURCE_LINE
