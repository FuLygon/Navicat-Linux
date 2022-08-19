#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <filesystem>
#include <optional>

#include <fmt/format.h>

#include "resource_wrapper.hpp"
#include "resource_traits/cxx_object_traits.hpp"
#include "resource_traits/unix_os/file_descriptor.hpp"
#include "resource_traits/unix_os/map_view.hpp"

#include "rsa_cipher.hpp"
#include "elf64_interpreter.hpp"
#include "patch_solution.hpp"
#include "patch_solution_since_16.0.7.0.hpp"

#include "exception.hpp"
#include "exceptions/unix_exception.hpp"
#include "exceptions/operation_canceled_exception.hpp"

#define NKG_CURRENT_SOURCE_FILE() ".\\navicat-patcher\\main.cpp"
#define NKG_CURRENT_SOURCE_LINE() __LINE__

void welcome() {
    puts("***************************************************");
    puts("*       navicat-patcher by @DoubleLabyrinth       *");
    puts("*               version: 16.0.7.0-3               *");
    puts("***************************************************");
    puts("");
}

void help() {
    puts("Usage:");
    puts("    navicat-patcher [--dry-run] <Navicat root directory> [RSA-2048 private key file]");
    puts("");
    puts("    [--dry-run]                   Run patcher without applying any patches.");
    puts("                                  This parameter is optional.");
    puts("");
    puts("    <Navicat root directory>      Path to a directory where Navicat locates.");
    puts("                                  This parameter is mandatory.");
    puts("");
    puts("    [RSA-2048 private key file]   Path to an RSA-2048 private key file.");
    puts("                                  If not specified, an RSA-2048 private key file");
    puts("                                      named \"RegPrivateKey.pem\" will be generated.");
    puts("                                  This parameter is optional.");
    puts("");
    puts("Example:");
    puts("    ./navicat-patcher ~/navicat16-premium-en-patched");
    puts("");
}

bool parse_cmdline(int argc, char* argv[], bool& dry_run, std::filesystem::path& navicat_root, std::filesystem::path& rsa_keyfile) {
    if (argc == 2) {
        dry_run = false;
        navicat_root = argv[1];
        rsa_keyfile.clear();
        return true;
    } else if (argc == 3) {
        if (strcmp(argv[1], "--dry-run") == 0) {
            dry_run = true;
            navicat_root = argv[2];
            rsa_keyfile.clear();
            return true;
        } else {
            dry_run = false;
            navicat_root = argv[1];
            rsa_keyfile = argv[2];
            return true;
        }
    } else if (argc == 4) {
        if (strcmp(argv[1], "--dry-run") == 0) {
            dry_run = true;
            navicat_root = argv[2];
            rsa_keyfile = argv[3];
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void select_patch_solutions(nkg::resource_wrapper<nkg::resource_traits::cxx_object_traits<nkg::patch_solution>>& solution0) {
    return;
}

void load_rsa_privkey(nkg::rsa_cipher& cipher, std::filesystem::path& rsa_key_file, nkg::patch_solution* solution0) {
    if (!rsa_key_file.empty()) {
        printf("[*] Import RSA-2048 private key from\n");
        printf("    %s\n", rsa_key_file.native().c_str());

        cipher.import_private_key_file(rsa_key_file.native());

        if (solution0 && !solution0->check_rsa_privkey(cipher)) {
            throw nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "The RSA private key you provide cannot be used.");
        }
    } else {
        printf("[*] Generating new RSA private key, it may take a long time...\n");

        do {
            cipher.generate_key(2048);
        } while (solution0 && !solution0->check_rsa_privkey(cipher));   // re-generate RSA key if one of `check_rsa_privkey` returns false
    }

    printf("[*] Your RSA private key:\n%s\n", cipher.export_private_key_string().c_str());
}

template<typename... args_t>
bool all_patch_solutions_are_suppressed(args_t&&... args) {
    return (!args.is_valid() && ...);
}

void detect_backup(const std::filesystem::path& file_path) {
    std::filesystem::path backup_path = file_path.native() + ".bak";
    if (std::filesystem::is_regular_file(backup_path)) {
        while (true) {
            printf("[*] Previous backup %s is detected. Delete? (y/n)", backup_path.native().c_str());

            auto select = getchar();
            while (select != '\n' && getchar() != '\n') {}

            if (select == 'Y' || select == 'y') {
                std::filesystem::remove(backup_path);
                break;
            } else if (select == 'N' || select == 'n') {
                throw nkg::exceptions::operation_canceled_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Backup file still exists. Patch abort!");
            } else {
                continue;
            }
        }
    }
}

void make_backup(const std::filesystem::path& file_path) {
    std::filesystem::path backup_path = file_path.native() + ".bak";
    if (std::filesystem::exists(backup_path)) {
        throw nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Previous backup is detected.")
            .push_hint(fmt::format("Please delete {} and try again.", backup_path.native()));
    } else {
        std::filesystem::copy_file(file_path, backup_path);
    }
}

int main(int argc, char* argv[]) {
    welcome();

    bool dry_run = false;
    std::filesystem::path navicat_root;
    std::filesystem::path rsa_key_file;

    if (!parse_cmdline(argc, argv, dry_run, navicat_root, rsa_key_file)) {
        help();
        return -1;
    }

    try {
        if (!std::filesystem::is_directory(navicat_root)) {
            throw nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Navicat root directory path doesn't point to a directory.")
                .push_hint("Are you sure the path you specified is correct?")
                .push_hint(fmt::format("The path you specified: {}", navicat_root.native()));
        }

        if (!rsa_key_file.empty() && !std::filesystem::is_regular_file(rsa_key_file)) {
            throw nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "RSA private key file path doesn't point to a file.")
                .push_hint("Are you sure the path you specified is correct?")
                .push_hint(fmt::format("The path you specified: {}", rsa_key_file.native()));
        }

        nkg::rsa_cipher cipher;

        std::filesystem::path libcc_filepath = navicat_root / "usr" / "lib" / "libcc.so";
        nkg::resource_wrapper libcc_fd{ nkg::resource_traits::unix_os::file_descriptor{} };
        nkg::resource_wrapper libcc_stat{ nkg::resource_traits::cxx_object_traits<struct stat>{} };
        nkg::resource_wrapper libcc_map_view{ nkg::resource_traits::unix_os::map_view{}, [&libcc_stat](void* p) { munmap(p, libcc_stat->st_size); } };
        std::optional<nkg::elf64_interpreter> libcc_interpreter;

        nkg::resource_wrapper solution0{ nkg::resource_traits::cxx_object_traits<nkg::patch_solution>{} };

        // open libcc.dll
        libcc_fd.set(open(libcc_filepath.native().c_str(), O_RDWR));
        if (libcc_fd.is_valid()) {
            printf("[+] Try to open libcc.dll ... OK!\n");
        } else {
            if (errno == ENOENT) {
                printf("[-] Try to open libcc.dll ... NOT FOUND!\n");
            } else {
                throw nkg::exceptions::unix_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), errno, "open failed.");
            }
        }

        if (libcc_fd.is_valid()) {
            libcc_stat.set(new struct stat());
            if (fstat(libcc_fd.get(), libcc_stat.get()) != 0) {
                throw nkg::exceptions::unix_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), errno, "fstat failed.");
            }

            libcc_map_view.set(mmap(nullptr, libcc_stat->st_size, PROT_READ | PROT_WRITE, MAP_SHARED, libcc_fd.get(), 0));
            if (!libcc_map_view.is_valid()) {
                throw nkg::exceptions::unix_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), errno, "mmap failed.");
            }

            libcc_interpreter = nkg::elf64_interpreter::parse(libcc_map_view.get(), libcc_stat->st_size);

            solution0.set(new nkg::patch_solution_since<16, 0, 7, 0>(libcc_interpreter.value()));
        }

        puts("");

        // find patch and decide which solution will be applied
        if (solution0.is_valid()) {
            auto patch_found = solution0->find_patch();
            puts("");

            if (!patch_found) {
                solution0.release();
            }
        }

        select_patch_solutions(solution0);

        if (all_patch_solutions_are_suppressed(solution0)) {
            throw nkg::exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "All patch solutions are suppressed. Patch abort!")
                .push_hint("Are you sure your navicat has not been patched/modified before?");
        }

        // load key
        load_rsa_privkey(cipher, rsa_key_file, solution0.get());

        // apply patch solutions
        if (dry_run) {
            puts("*******************************************************");
            puts("*               DRY-RUN MODE ENABLE!                  *");
            puts("*             NO PATCH WILL BE APPLIED!               *");
            puts("*******************************************************");
        } else {
            // save private key if not given
            if (rsa_key_file.empty()) {
                cipher.export_private_key_file("RegPrivateKey.pem");
            }

            // detecting backups
            if (solution0.is_valid()) {
                detect_backup(libcc_filepath);
            }

            // make backup
            if (solution0.is_valid()) {
                make_backup(libcc_filepath);
            }

            // make patch
            // no way to go back from here :-)
            if (solution0.is_valid()) {
                solution0->make_patch(cipher);
            }

            // print new key file path
            if (rsa_key_file.empty()) {
                printf("[*] New RSA-2048 private key has been saved to\n");
                printf("    %s\n", (std::filesystem::current_path() / "RegPrivateKey.pem").c_str());
            }

            puts("");
            puts("*******************************************************");
            puts("*           PATCH HAS BEEN DONE SUCCESSFULLY!         *");
            puts("*                  HAVE FUN AND ENJOY~                *");
            puts("*******************************************************");
        }

        return 0;
    } catch (nkg::exception& e) {
        printf("[-] %s:%d ->\n", e.source_file().c_str(), e.source_line());
        printf("    %s\n", e.custom_message().c_str());
        if (e.error_code_exists()) {
            printf("    %s (0x%zx)\n", e.error_string().c_str(), e.error_code());
        }

        for (auto& hint : e.hints()) {
            printf("    HINT: %s\n", hint.c_str());
        }

        return -1;
    }
}

#undef NKG_CURRENT_SOURCE_LINE
#undef NKG_CURRENT_SOURCE_FILE
