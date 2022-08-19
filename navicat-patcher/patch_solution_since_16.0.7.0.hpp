#pragma once
#include <string>
#include <list>
#include "patch_solution_since.hpp"
#include "elf64_interpreter.hpp"
#include "amd64_emulator.hpp"

namespace nkg {

    template<>
    class patch_solution_since<16, 0, 7, 0> final : public patch_solution {
    private:
        static inline std::string_view official_encoded_key = "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAw1dqF3SkCaAAmMzs889IqdW9M2dIdh3jG9yPcmLnmJiGpBF4E9VHSMGe8oPAy2kJDmdNt4BcEygvssEfginva5t5jm352UAoDosUJkTXGQhpAWMF4fBmBpO3EedG62rOsqMBgmSdAyxCSPBRJIOFR0QgZFbRnU0frj34fiVmgYiLuZSAmIbs8ZxiHPdp1oD4tUpvsFci4QJtYNjNnGU2WPH6rvChGl1IRKrxMtqLielsvajUjyrgOC6NmymYMvZNER3htFEtL1eQbCyTfDmtYyQ1Wt4Ot12lxf0wVIR5mcGN7XCXJRHOFHSf1gzXWabRSvmt1nrl7sW6cjxljuuQawIDAQAB";

        elf64_interpreter& m_libcc_interpreter;
        elf64_interpreter::va_t m_va_CSRegistrationInfoFetcher_LINUX_vtable;
        elf64_interpreter::va_t m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey;
        elf64_interpreter::va_t m_va_pltgot_std_string_append;
        std::list<std::pair<elf64_interpreter::va_t, size_t>> m_tracing;

        uint64_t _emulator_append_external_api_impl(amd64_emulator& x64_emulator, std::string_view api_name, const std::vector<uint8_t>& api_impl);

        uint64_t _emulator_malloc(amd64_emulator& x64_emulator, size_t alloc_size);

        void _emulator_free(amd64_emulator& x64_emulator, uint64_t alloc_p);

        bool _emulator_page_fault_handler(amd64_emulator& x64_emulator, uc_mem_type access, uint64_t address, size_t size, int64_t value);

        void _emulator_dl_runtime_resolve_handler(amd64_emulator& x64_emulator, uint64_t address, size_t size);

        void _emulator_malloc_handler(amd64_emulator& x64_emulator, uint64_t address, size_t size);

        void _emulator_free_handler(amd64_emulator& x64_emulator, uint64_t address, size_t size);

        static std::string _build_encoded_key(const rsa_cipher& cipher);

        std::list<std::pair<elf64_interpreter::va_t, size_t>> _calculate_reliable_areas() const;

    public:
        patch_solution_since(elf64_interpreter& libcc_interpreter);

        [[nodiscard]]
        virtual bool find_patch() override;

        [[nodiscard]]
        virtual bool check_rsa_privkey(const rsa_cipher& cipher) override;

        virtual void make_patch(const rsa_cipher& cipher) override;

    };

}
