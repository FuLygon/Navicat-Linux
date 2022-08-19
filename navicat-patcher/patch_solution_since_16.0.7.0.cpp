#include "patch_solution_since_16.0.7.0.hpp"
#include <regex>
#include "keystone_assembler.hpp"
#include <fmt/format.h>

namespace nkg {

    uint64_t patch_solution_since<16, 0, 7, 0>::_emulator_append_external_api_impl(amd64_emulator& x64_emulator, std::string_view api_name, const std::vector<uint8_t>& api_impl) {
        auto& external_api_impl = x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl");
        auto& external_api_impl_area_base = x64_emulator.context_get<uint64_t&>("external_api_impl_area_base");
        auto& external_api_impl_area_size = x64_emulator.context_get<size_t&>("external_api_impl_area_size");
        auto& external_api_impl_append_address = x64_emulator.context_get<uint64_t&>("external_api_impl_append_address");

        auto p = external_api_impl_append_address;
        if (p + api_impl.size() > external_api_impl_area_base + external_api_impl_area_size) {
            auto expand_size = ((p + api_impl.size()) - (external_api_impl_area_base + external_api_impl_area_size) + 0xfff) / 0x1000 * 0x1000;
            x64_emulator.mem_map(external_api_impl_area_base + external_api_impl_area_size, expand_size, UC_PROT_READ | UC_PROT_EXEC);
            external_api_impl_area_size += expand_size;
        }

        x64_emulator.mem_write(p, api_impl);

        external_api_impl[std::string(api_name)] = p;
        external_api_impl_append_address = (p + api_impl.size() + 0xf) / 0x10 * 0x10;
        return p;
    }

    uint64_t patch_solution_since<16, 0, 7, 0>::_emulator_malloc(amd64_emulator& x64_emulator, size_t alloc_size) {
        auto& heap_records = x64_emulator.context_get<std::map<uint64_t, uint64_t>&>("heap_records");

        auto predecessor_chunk =
            std::adjacent_find(heap_records.begin(), heap_records.end(), [alloc_size](auto& chunk0, auto& chunk1) { return chunk1.first - (chunk0.first + chunk0.second) >= alloc_size; });

        uint64_t alloc_p;
        if (predecessor_chunk != heap_records.end()) {
            alloc_p = predecessor_chunk->first + predecessor_chunk->second;
        } else {
            auto heap_base = x64_emulator.context_get<uint64_t>("heap_base");
            auto heap_size = x64_emulator.context_get<uint64_t>("heap_size");

            auto free_space_base = heap_records.empty() ? heap_base : heap_records.rbegin()->first + heap_records.rbegin()->second;
            auto free_space_size = heap_base + heap_size - free_space_base;

            if (free_space_size < alloc_size) {
                auto heap_expand_base = heap_base + heap_size;
                auto heap_expand_size = (alloc_size - free_space_size + 0xfff) / 0x1000 * 0x1000;
                x64_emulator.mem_map(heap_expand_base, heap_expand_size, UC_PROT_READ | UC_PROT_WRITE);
            }

            alloc_p = free_space_base;
        }

        heap_records[alloc_p] = alloc_size;

        return alloc_p;
    }

    void patch_solution_since<16, 0, 7, 0>::_emulator_free(amd64_emulator& x64_emulator, uint64_t alloc_p) {
        auto& heap_records = x64_emulator.context_get<std::map<uint64_t, uint64_t>&>("heap_records");

        auto chunk = heap_records.find(alloc_p);
        if (chunk != heap_records.end()) {
            heap_records.erase(chunk);
        } else {
            printf("[-] patch_solution_since<16, 0, 7, 0>: emulator tries to free 0x%016lx which is not allocated by malloc.\n", alloc_p);
            x64_emulator.emu_stop();
        }
    }

    bool patch_solution_since<16, 0, 7, 0>::_emulator_page_fault_handler(amd64_emulator& x64_emulator, uc_mem_type access, uint64_t address, size_t size, int64_t value) {
        try {
            auto fault_segment = m_libcc_interpreter.elf_program_header_from_va(address);

            auto page_base = address / 0x1000 * 0x1000;
            auto page_size = 0x1000;
            uint32_t page_perms = UC_PROT_NONE;

            if (fault_segment->p_flags & PF_R) {
                page_perms |= UC_PROT_READ;
            }
            if (fault_segment->p_flags & PF_W) {
                page_perms |= UC_PROT_WRITE;
            }
            if (fault_segment->p_flags & PF_X) {
                page_perms |= UC_PROT_EXEC;
            }

            x64_emulator.mem_map(page_base, page_size, page_perms);
            x64_emulator.mem_write(page_base, m_libcc_interpreter.convert_va_to_ptr<const void*>(page_base), page_size);

            auto dynamic_pltgot = m_libcc_interpreter.elf_dynamic_pltgot();
            if (dynamic_pltgot.has_value() && page_base <= dynamic_pltgot.value() + 0x8 && dynamic_pltgot.value() + 0x10 <= page_base + page_size) {
                uint64_t dead_address = x64_emulator.context_get<uint64_t>("dead_address");
                x64_emulator.mem_write(dynamic_pltgot.value() + 0x8, &dead_address, sizeof(dead_address));
            }

            if (dynamic_pltgot.has_value() && page_base <= dynamic_pltgot.value() + 0x10 && dynamic_pltgot.value() + 0x18 <= page_base + page_size) {
                uint64_t dl_runtime_resolve = x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["dl_runtime_resolve"];
                x64_emulator.mem_write(dynamic_pltgot.value() + 0x10, &dl_runtime_resolve, sizeof(dl_runtime_resolve));
            }

            return true;
        } catch (::nkg::exception&) {
            return false;
        }
    }

    void patch_solution_since<16, 0, 7, 0>::_emulator_dl_runtime_resolve_handler(amd64_emulator& x64_emulator, uint64_t address, size_t size) {
        uint64_t rsp;
        x64_emulator.reg_read(UC_X86_REG_RSP, &rsp);

        uint64_t reloc_idx;
        x64_emulator.mem_read(rsp + 0x8, &reloc_idx, sizeof(reloc_idx));

        uint64_t reloc_va;
        Elf64_Sym* reloc_sym;
        uint32_t reloc_type;
        char* reloc_sym_name;
        if (m_libcc_interpreter.elf_dynamic_pltrel().value() == DT_REL) {
            auto jmp_reloc_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Rel*>(m_libcc_interpreter.elf_dynamic_jmprel().value());
            auto symbol_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Sym*>(m_libcc_interpreter.elf_dynamic_symtab().value());
            auto string_table = m_libcc_interpreter.convert_va_to_ptr<char*>(m_libcc_interpreter.elf_dynamic_strtab().value());
            reloc_va = jmp_reloc_table[reloc_idx].r_offset;
            reloc_sym = &symbol_table[ELF64_R_SYM(jmp_reloc_table[reloc_idx].r_info)];
            reloc_type = ELF64_R_TYPE(jmp_reloc_table[reloc_idx].r_info);
            reloc_sym_name = &string_table[reloc_sym->st_name];
        } else {    // m_libcc_interpreter.elf_dynamic_pltrel().value() == DT_RELA
            auto jmp_reloc_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Rela*>(m_libcc_interpreter.elf_dynamic_jmprel().value());
            auto symbol_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Sym*>(m_libcc_interpreter.elf_dynamic_symtab().value());
            auto string_table = m_libcc_interpreter.convert_va_to_ptr<char*>(m_libcc_interpreter.elf_dynamic_strtab().value());
            reloc_va = jmp_reloc_table[reloc_idx].r_offset;
            reloc_sym = &symbol_table[ELF64_R_SYM(jmp_reloc_table[reloc_idx].r_info)];
            reloc_type = ELF64_R_TYPE(jmp_reloc_table[reloc_idx].r_info);
            reloc_sym_name = &string_table[reloc_sym->st_name];
        }

        if (strcmp(reloc_sym_name, "strlen") == 0) {
            auto external_api_impl_va =
                x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["strlen"];

            x64_emulator.mem_write(reloc_va, &external_api_impl_va, sizeof(external_api_impl_va));

            // external api address is resolved, set `qword ptr [rsp] = external_api_impl_va` in order to jump there
            x64_emulator.mem_write(rsp, &external_api_impl_va, sizeof(external_api_impl_va));
        } else if (strcmp(reloc_sym_name, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm") == 0) {
            // std::string::_M_append(char const*, unsigned long)
            auto external_api_impl_va =
                x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm"];

            x64_emulator.mem_write(reloc_va, &external_api_impl_va, sizeof(external_api_impl_va));

            // external api address is resolved, set `qword ptr [rsp] = external_api_impl_va` in order to jump there
            x64_emulator.mem_write(rsp, &external_api_impl_va, sizeof(external_api_impl_va));
        } else if (strcmp(reloc_sym_name, "_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7compareEPKc") == 0) {
            // std::string::compare(char const*) const
            auto external_api_impl_va =
                x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7compareEPKc"];

            x64_emulator.mem_write(reloc_va, &external_api_impl_va, sizeof(external_api_impl_va));

            // external api address is resolved, set `qword ptr [rsp] = external_api_impl_va` in order to jump there
            x64_emulator.mem_write(rsp, &external_api_impl_va, sizeof(external_api_impl_va));
        } else if (strcmp(reloc_sym_name, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9push_backEc") == 0) {
            // std::string::push_back(char)
            auto external_api_impl_va =
                x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9push_backEc"];

            x64_emulator.mem_write(reloc_va, &external_api_impl_va, sizeof(external_api_impl_va));

            // external api address is resolved, set `qword ptr [rsp] = external_api_impl_va` in order to jump there
            x64_emulator.mem_write(rsp, &external_api_impl_va, sizeof(external_api_impl_va));
        } else if (strcmp(reloc_sym_name, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc") == 0) {
            // std::string::append(char const*)
            m_va_pltgot_std_string_append = reloc_va;

            auto external_api_impl_va =
                x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc"];

            x64_emulator.mem_write(reloc_va, &external_api_impl_va, sizeof(external_api_impl_va));

            // external api address is resolved, set `qword ptr [rsp] = external_api_impl_va` in order to jump there
            x64_emulator.mem_write(rsp, &external_api_impl_va, sizeof(external_api_impl_va));
        } else if (strcmp(reloc_sym_name, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKcm") == 0) {
            // std::string::append(char const*, unsigned long)

            // redirect to `std::string::_M_append(char const*, unsigned long)`
            auto external_api_impl_va =
                x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm"];

            x64_emulator.mem_write(reloc_va, &external_api_impl_va, sizeof(external_api_impl_va));

            // external api address is resolved, set `qword ptr [rsp] = external_api_impl_va` in order to jump there
            x64_emulator.mem_write(rsp, &external_api_impl_va, sizeof(external_api_impl_va));
        } else if (strcmp(reloc_sym_name, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv") == 0) {
            // std::string::_M_dispose()
            auto external_api_impl_va =
                x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl")["_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv"];

            x64_emulator.mem_write(reloc_va, &external_api_impl_va, sizeof(external_api_impl_va));

            // external api address is resolved, set `qword ptr [rsp] = external_api_impl_va` in order to jump there
            x64_emulator.mem_write(rsp, &external_api_impl_va, sizeof(external_api_impl_va));
        } else {
            printf("[-] patch_solution_since<16, 0, 7, 0>: PLT GOT entry `%s` is not resolved.\n", reloc_sym_name);
            x64_emulator.emu_stop();
        }
    }

    void patch_solution_since<16, 0, 7, 0>::_emulator_malloc_handler(amd64_emulator& x64_emulator, uint64_t address, size_t size) {
        uint64_t rdi;
        x64_emulator.reg_read(UC_X86_REG_RDI, &rdi);

        uint64_t rax = _emulator_malloc(x64_emulator, rdi);
        x64_emulator.reg_write(UC_X86_REG_RAX, &rax);
    }

    void patch_solution_since<16, 0, 7, 0>::_emulator_free_handler(amd64_emulator& x64_emulator, uint64_t address, size_t size) {
        uint64_t rdi;
        x64_emulator.reg_read(UC_X86_REG_RDI, &rdi);
        _emulator_free(x64_emulator, rdi);
    }

    std::string patch_solution_since<16, 0, 7, 0>::_build_encoded_key(const rsa_cipher& cipher) {
        auto encoded_key = cipher.export_public_key_string_pem();

        encoded_key = std::regex_replace(encoded_key, std::regex("-----BEGIN PUBLIC KEY-----"), "");
        encoded_key = std::regex_replace(encoded_key, std::regex("-----END PUBLIC KEY-----"), "");
        encoded_key = std::regex_replace(encoded_key, std::regex("\n"), "");

        return encoded_key;
    }

    std::list<std::pair<elf64_interpreter::va_t, size_t>> patch_solution_since<16, 0, 7, 0>::_calculate_reliable_areas() const {
        std::list<std::pair<elf64_interpreter::va_t, size_t>> reliable_areas;

        for (const auto& [base, size] : m_tracing) {
            auto tracing_block_begin_va = base;
            auto tracing_block_end_va = base + size;

            auto va = tracing_block_begin_va;
            auto next_reloc = m_libcc_interpreter.relocation_distribute().lower_bound(va);

            while (va < tracing_block_end_va) {
                if (next_reloc == m_libcc_interpreter.relocation_distribute().end() || tracing_block_end_va <= next_reloc->first) {
                    reliable_areas.emplace_back(std::make_pair(va, static_cast<size_t>(tracing_block_end_va - va)));
                    break;
                } else {
                    reliable_areas.emplace_back(std::make_pair(va, static_cast<size_t>(next_reloc->first - va)));
                    va = next_reloc->first + next_reloc->second;
                    ++next_reloc;
                }
            }
        }

        return reliable_areas;
    }

    patch_solution_since<16, 0, 7, 0>::patch_solution_since(elf64_interpreter& libcc_interpreter) :
        m_libcc_interpreter(libcc_interpreter),
        m_va_CSRegistrationInfoFetcher_LINUX_vtable(0),
        m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey(0),
        m_va_pltgot_std_string_append(0) {}

    bool patch_solution_since<16, 0, 7, 0>::find_patch() {
        if (m_libcc_interpreter.elf_header()->e_machine != EM_X86_64) {
            printf("[-] patch_solution_since<16, 0, 7, 0>: Not x86-64 elf binary.\n");
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
            return false;
        }

        auto CSRegistrationInfoFetcher_LINUX_typeinfo_name =
            m_libcc_interpreter.search_section<uint8_t*>(
                ".rodata",
                [](const uint8_t* base, size_t off, size_t size) {
                    return (size - off) >= sizeof("31CSRegistrationInfoFetcher_LINUX") && strcmp(address_offset_cast<const char*>(base, off), "31CSRegistrationInfoFetcher_LINUX") == 0;
                }
            );

        if (CSRegistrationInfoFetcher_LINUX_typeinfo_name == nullptr) {
            printf("[-] patch_solution_since<16, 0, 7, 0>: RTTI info for CSRegistrationInfoFetcher_LINUX is not found. (failure label 0)\n");
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
            return false;
        }

        auto CSRegistrationInfoFetcher_LINUX_typeinfo_name_va =
            m_libcc_interpreter.convert_ptr_to_va(CSRegistrationInfoFetcher_LINUX_typeinfo_name);

        auto CSRegistrationInfoFetcher_LINUX_typeinfo =
            m_libcc_interpreter.search_section<uint8_t*>(
                ".data.rel.ro",
                [CSRegistrationInfoFetcher_LINUX_typeinfo_name_va](const uint8_t* base, size_t off, size_t size) {
                    return off % 8 == 0 && (size - (off + 8)) >= 8 && *address_offset_cast<const uint64_t*>(base, off + 8) == CSRegistrationInfoFetcher_LINUX_typeinfo_name_va;
                }
            );

        if (CSRegistrationInfoFetcher_LINUX_typeinfo == nullptr) {
            printf("[-] patch_solution_since<16, 0, 7, 0>: RTTI info for CSRegistrationInfoFetcher_LINUX is not found. (failure label 1)\n");
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
            return false;
        }

        auto CSRegistrationInfoFetcher_LINUX_typeinfo_va =
            m_libcc_interpreter.convert_ptr_to_va(CSRegistrationInfoFetcher_LINUX_typeinfo);

        auto CSRegistrationInfoFetcher_LINUX_vftable_before =
            m_libcc_interpreter.search_section<uint64_t*>(
                ".data.rel.ro",
                [CSRegistrationInfoFetcher_LINUX_typeinfo_va](const uint8_t* base, size_t off, size_t size) {
                    return off % 8 == 0 && (size - off) >= 8 && *address_offset_cast<const uint64_t*>(base, off) == CSRegistrationInfoFetcher_LINUX_typeinfo_va;
                }
            );

        if (CSRegistrationInfoFetcher_LINUX_vftable_before == nullptr) {
            printf("[-] patch_solution_since<16, 0, 7, 0>: Vftable for CSRegistrationInfoFetcher_LINUX is not found.\n");
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
        }

        auto CSRegistrationInfoFetcher_LINUX_vftable = CSRegistrationInfoFetcher_LINUX_vftable_before + 1;

        m_va_CSRegistrationInfoFetcher_LINUX_vtable = m_libcc_interpreter.convert_ptr_to_va(CSRegistrationInfoFetcher_LINUX_vftable);
        m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey = CSRegistrationInfoFetcher_LINUX_vftable[7];
        printf("[*] patch_solution_since<16, 0, 7, 0>: m_va_CSRegistrationInfoFetcher_LINUX_vtable = 0x%016lx\n", m_va_CSRegistrationInfoFetcher_LINUX_vtable);
        printf("[*] patch_solution_since<16, 0, 7, 0>: m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey = 0x%016lx\n", m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey);

        amd64_emulator x64_emulator;

        x64_emulator.context_set("heap_base", uint64_t{ 0x00007fff00000000 });
        x64_emulator.context_set("heap_size", size_t{ 0x1000 * 32 });
        x64_emulator.context_set("heap_records", std::map<uint64_t, uint64_t>{});

        x64_emulator.context_set("stack_base", uint64_t{ 0x00007fffffff0000 });
        x64_emulator.context_set("stack_size", size_t{ 0x1000 * 32 });
        x64_emulator.context_set("stack_top", uint64_t{ x64_emulator.context_get<uint64_t>("stack_base") - x64_emulator.context_get<size_t>("stack_size") });

        x64_emulator.context_set("dead_area_base", uint64_t{ 0xfffffffffffff000 });
        x64_emulator.context_set("dead_area_size", size_t{ 0x1000 });

        x64_emulator.context_set("external_api_impl", std::map<std::string, uint64_t>{});
        x64_emulator.context_set("external_api_impl_area_base", uint64_t{ 0xffff900000000000 });
        x64_emulator.context_set("external_api_impl_area_size", size_t{ 0 });
        x64_emulator.context_set("external_api_impl_append_address", x64_emulator.context_get<uint64_t>("external_api_impl_area_base"));

        x64_emulator.context_set("start_address", static_cast<uint64_t>(m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey));
        x64_emulator.context_set("dead_address", x64_emulator.context_get<uint64_t>("dead_area_base"));

        // allocate heap
        x64_emulator.mem_map(x64_emulator.context_get<uint64_t>("heap_base"), x64_emulator.context_get<size_t>("heap_size"), UC_PROT_READ | UC_PROT_WRITE);

        // allocate stack
        x64_emulator.mem_map(x64_emulator.context_get<uint64_t>("stack_top"), x64_emulator.context_get<size_t>("stack_size"), UC_PROT_READ | UC_PROT_WRITE);

        // allocate dead area
        x64_emulator.mem_map(x64_emulator.context_get<uint64_t>("dead_area_base"), x64_emulator.context_get<size_t>("dead_area_size"), UC_PROT_READ | UC_PROT_EXEC);

        // allocate and setup external api impl area
        {
            keystone_assembler x64_assembler{ KS_ARCH_X86, KS_MODE_64 };

            auto& external_api_impl = x64_emulator.context_get<std::map<std::string, uint64_t>&>("external_api_impl");
            auto& external_api_impl_append_address = x64_emulator.context_get<uint64_t&>("external_api_impl_append_address");

            _emulator_append_external_api_impl
                (
                    x64_emulator, "dl_runtime_resolve",
                    x64_assembler.assemble
                        (
                            "    nop;"
                            "    mov rax, qword ptr [rsp];"
                            "    add rsp, 0x10;"
                            "    cmp rax, 0;"
                            "    je just_ret;"
                            "    jmp rax;"
                            "just_ret:"
                            "    ret;"
                        )
                );

            _emulator_append_external_api_impl(x64_emulator, "malloc", x64_assembler.assemble("ret;"));

            _emulator_append_external_api_impl(x64_emulator, "free", x64_assembler.assemble("ret;"));

            _emulator_append_external_api_impl
                (
                    x64_emulator, "strlen",
                    x64_assembler.assemble
                        (
                            "xor rcx, rcx;"
                            "dec rcx;"
                            "xor rax, rax;"
                            "repne scasb byte ptr [rdi];"
                            "not rcx;"
                            "dec rcx;"
                            "mov rax, rcx;"
                            "ret;"
                        )
                );

            _emulator_append_external_api_impl
                (
                    x64_emulator, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm",
                    x64_assembler.assemble
                        (
                            fmt::format
                                (
                                    "    push r12;"
                                    "    push r13;"
                                    "    push r14;"
                                    "    push r15;"
                                    "    mov r12, qword ptr [rdi];"
                                    "    mov r13, qword ptr [rdi + 0x8];"
                                    "    mov r14, 15;"
                                    "    lea rax, qword ptr [rdi + 0x10];"
                                    "    cmp r12, rax;"
                                    "    cmovne r14, qword ptr [rdi + 0x10];"
                                    "    xor r15, r15;"
                                    "    lea rax, qword ptr [r13 + rdx];"
                                    "    cmp rax, r14;"
                                    "    jbe append_string;"
                                    "reallocate:"
                                    "    push rdi;"
                                    "    push rsi;"
                                    "    push rdx;"
                                    "    lea r14, qword ptr [r13 + rdx];"
                                    "    lea rdi, qword ptr [r14 + 0x1];"
                                    "    mov rax, {malloc:#016x};"
                                    "    call rax;"
                                    "    mov rdi, rax;"
                                    "    mov rsi, r12;"
                                    "    mov rcx, r13;"
                                    "    rep movs byte ptr [rdi], byte ptr [rsi];"
                                    "    mov r12, rax;"
                                    "    mov rdi, qword ptr [rsp + 0x10];"
                                    "    lea rax, qword ptr [rdi + 0x10];"
                                    "    mov rdi, qword ptr [rdi];"
                                    "    cmp rdi, rax;"
                                    "    je label_0;"
                                    "    mov rax, {free:#016x};"
                                    "    call rax;"
                                    "label_0:"
                                    "    pop rdx;"
                                    "    pop rsi;"
                                    "    pop rdi;"
                                    "    mov r15b, 0x1;"
                                    "append_string:"
                                    "    push rdi;"
                                    "    push rsi;"
                                    "    lea rdi, qword ptr [r12 + r13];"
                                    "    mov rcx, rdx;"
                                    "    rep movs byte ptr [rdi], byte ptr [rsi];"
                                    "    mov byte ptr [rdi], 0;"
                                    "    pop rsi;"
                                    "    pop rdi;"
                                    "    add r13, rdx;"
                                    "update_string_struct:"
                                    "    mov qword ptr [rdi + 0x8], r13;"
                                    "    test r15, r15;"
                                    "    jz final;"
                                    "    mov qword ptr [rdi], r12;"
                                    "    mov qword ptr [rdi + 0x10], r14;"
                                    "final:"
                                    "    mov rax, rdi;"
                                    "    pop r15;"
                                    "    pop r14;"
                                    "    pop r13;"
                                    "    pop r12;"
                                    "    ret;",
                                    fmt::arg("malloc", external_api_impl["malloc"]),
                                    fmt::arg("free", external_api_impl["free"])
                                )
                        )
                );

            _emulator_append_external_api_impl
                (
                    x64_emulator, "_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7compareEPKc",
                    x64_assembler.assemble
                        (
                            // rcx = strlen(rsi);
                            "    push rsi;"
                            "    xor rcx, rcx;"
                            "    dec rcx;"
                            "strlen_loop:"
                            "    inc rcx;"
                            "    cmp byte ptr [rsi + rcx], 0x00;"
                            "    jne strlen_loop;"
                            "    pop rsi;"
                            // rdx = rcx;
                            // rcx = min(this->_M_string_length, rcx);
                            "    mov rdx, rcx;"
                            "    cmp qword ptr [rdi + 0x8], rcx;"
                            "    cmovb rcx, qword ptr [rdi + 0x8];"
                            // if (rcx == 0 || memcmp(this->_M_dataplus, rsi, rcx) == 0) goto compare_length;
                            "    test rcx, rcx;"
                            "    jz compare_length;"
                            "memcmp:"
                            "    push rdi;"
                            "    push rsi;"
                            "    mov rdi, qword ptr [rdi];"
                            "    xchg rsi, rdi;"
                            "    repe cmps byte ptr [rsi], byte ptr [rdi];"
                            "    pop rsi;"
                            "    pop rdi;"
                            "    jl return_negative;"
                            "    jg return_positive;"
                            "compare_length:"
                            "    cmp qword ptr [rdi + 0x8], rdx;"
                            "    ja return_positive;"
                            "    jb return_negative;"
                            "return_zero:"
                            "    xor eax, eax;"
                            "    ret;"
                            "return_positive:"
                            "    mov eax, 1;"
                            "    ret;"
                            "return_negative:"
                            "    mov eax, 0xffffffff;"
                            "    ret;"
                        )
                );

            _emulator_append_external_api_impl
                (
                    x64_emulator, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9push_backEc",
                    x64_assembler.assemble
                        (
                            fmt::format
                                (
                                    "    push rsi;"
                                    "    mov rsi, rsp;"
                                    "    mov rdx, 0x1;"
                                    "    mov rax, {_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm:#016x};"
                                    "    call rax;"
                                    "    pop rsi;"
                                    "    ret;",
                                    fmt::arg("_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm", external_api_impl["_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm"])
                                )
                        )
                );

            _emulator_append_external_api_impl
                (
                    x64_emulator, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc",
                    x64_assembler.assemble
                        (
                            fmt::format
                                (
                                    "    push rsi;"
                                    "    xor rdx, rdx;"
                                    "    dec rdx;"
                                    "strlen_loop:"
                                    "    inc rdx;"
                                    "    cmp byte ptr [rsi + rdx], 0x00;"
                                    "    jne strlen_loop;"
                                    "    pop rsi;"
                                    "    mov rax, {_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm:#016x};"
                                    "    jmp rax;",
                                    fmt::arg("_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm", external_api_impl["_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcm"])
                                )
                        )
                );

            _emulator_append_external_api_impl
                (
                    x64_emulator, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv",
                    x64_assembler.assemble
                        (
                            fmt::format
                                (
                                    "    mov rcx, qword ptr [rdi];"
                                    "    lea rdx, qword ptr [rdi + 0x10];"
                                    "    cmp rcx, rdx;"
                                    "    je return;"
                                    "    mov rdi, rcx;"
                                    "    mov rax, {free:#016x};"
                                    "    call rax;"
                                    "return:"
                                    "    ret",
                                    fmt::arg("free", external_api_impl["free"])
                                )
                        )
                );

            x64_emulator.hook_add<UC_HOOK_CODE>
                (std::bind(&patch_solution_since::_emulator_dl_runtime_resolve_handler, this, std::ref(x64_emulator), std::placeholders::_1, std::placeholders::_2), external_api_impl["dl_runtime_resolve"], external_api_impl["dl_runtime_resolve"]);

            x64_emulator.hook_add<UC_HOOK_CODE>
                (std::bind(&patch_solution_since::_emulator_malloc_handler, this, std::ref(x64_emulator), std::placeholders::_1, std::placeholders::_2), external_api_impl["malloc"], external_api_impl["malloc"]);

            x64_emulator.hook_add<UC_HOOK_CODE>
                (std::bind(&patch_solution_since::_emulator_free_handler, this, std::ref(x64_emulator), std::placeholders::_1, std::placeholders::_2), external_api_impl["free"], external_api_impl["free"]);
        }

        // set page fault handler
        x64_emulator.hook_add<UC_HOOK_MEM_UNMAPPED>
            (std::bind(&patch_solution_since::_emulator_page_fault_handler, this, std::ref(x64_emulator), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

        // set rbp, rsp
        uint64_t init_rbp = x64_emulator.context_get<uint64_t>("stack_base") - x64_emulator.context_get<size_t>("stack_size") / 4;
        uint64_t init_rsp = x64_emulator.context_get<uint64_t>("stack_base") - x64_emulator.context_get<size_t>("stack_size") / 2;

        x64_emulator.reg_write(UC_X86_REG_RBP, &init_rbp);
        x64_emulator.reg_write(UC_X86_REG_RSP, &init_rsp);

        // set return address
        auto retaddr = x64_emulator.context_get<uint64_t>("dead_address");
        x64_emulator.mem_write(init_rsp, &retaddr, sizeof(retaddr));

        // set argument registers
        uint64_t init_rdi = init_rsp + 0x40;    // a pointer to stack memory which stores return value
        x64_emulator.reg_write(UC_X86_REG_RDI, &init_rdi);

        std::list<std::pair<elf64_interpreter::va_t, size_t>> tracing;
        x64_emulator.hook_add<UC_HOOK_CODE>(
            [&x64_emulator, &tracing, stack_frame_index = 0](uint64_t address, size_t size) mutable {
                std::byte instruction_data[16]; // a x86-64 instruction is 16-bytes long at most
                x64_emulator.mem_read(address, instruction_data, size);

                if (stack_frame_index == 0) {
                    if (tracing.size() > 0 && tracing.back().first + tracing.back().second == address) {
                        tracing.back().second += size;
                    } else {
                        tracing.emplace_back(std::make_pair(address, size));
                    }
                }

                bool is_near_call = 
                    instruction_data[0] == std::byte{ 0xe8 } ||
                    instruction_data[0] == std::byte{ 0xff } && (instruction_data[1] & std::byte{ 0b00111000 }) == std::byte{ 0b00010000 }; // FF /2

                if (is_near_call) {
                    ++stack_frame_index;
                    return;
                }

                bool is_near_ret = 
                    instruction_data[0] == std::byte{ 0xc2 } ||
                    instruction_data[0] == std::byte{ 0xc3 };

                if (is_near_ret) {
                    --stack_frame_index;
                    return;
                }
            }
        );

        // start emulate
        try {
            x64_emulator.emu_start(x64_emulator.context_get<uint64_t>("start_address"), x64_emulator.context_get<uint64_t>("dead_address"));
        } catch (amd64_emulator::backend_error& e) {
            printf("[-] patch_solution_since<16, 0, 7, 0>: Code emulation failed. %s\n", e.error_string().c_str());
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
            return false;
        }

        if (m_va_pltgot_std_string_append == 0) {
            if (m_libcc_interpreter.elf_dynamic_pltrel().value() == DT_REL) {
                auto jmp_reloc_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Rel*>(m_libcc_interpreter.elf_dynamic_jmprel().value());
                auto jmp_reloc_table_size = m_libcc_interpreter.elf_dynamic_pltrelsz().value() / sizeof(Elf64_Rel);
                auto symbol_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Sym*>(m_libcc_interpreter.elf_dynamic_symtab().value());
                auto string_table = m_libcc_interpreter.convert_va_to_ptr<char*>(m_libcc_interpreter.elf_dynamic_strtab().value());

                for (size_t i = 0; i < jmp_reloc_table_size; ++i) {
                    auto reloc_va = jmp_reloc_table[i].r_offset;
                    auto reloc_sym = &symbol_table[ELF64_R_SYM(jmp_reloc_table[i].r_info)];
                    auto reloc_type = ELF64_R_TYPE(jmp_reloc_table[i].r_info);
                    auto reloc_sym_name = &string_table[reloc_sym->st_name];

                    if (strcmp(reloc_sym_name, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc") == 0) {
                        m_va_pltgot_std_string_append = reloc_va;
                        break;
                    }
                }
            } else {    // m_libcc_interpreter.elf_dynamic_pltrel().value() == DT_RELA
                auto jmp_reloc_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Rela*>(m_libcc_interpreter.elf_dynamic_jmprel().value());
                auto jmp_reloc_table_size = m_libcc_interpreter.elf_dynamic_pltrelsz().value() / sizeof(Elf64_Rela);
                auto symbol_table = m_libcc_interpreter.convert_va_to_ptr<Elf64_Sym*>(m_libcc_interpreter.elf_dynamic_symtab().value());
                auto string_table = m_libcc_interpreter.convert_va_to_ptr<char*>(m_libcc_interpreter.elf_dynamic_strtab().value());

                for (size_t i = 0; i < jmp_reloc_table_size; ++i) {
                    auto reloc_va = jmp_reloc_table[i].r_offset;
                    auto reloc_sym = &symbol_table[ELF64_R_SYM(jmp_reloc_table[i].r_info)];
                    auto reloc_type = ELF64_R_TYPE(jmp_reloc_table[i].r_info);
                    auto reloc_sym_name = &string_table[reloc_sym->st_name];

                    if (strcmp(reloc_sym_name, "_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc") == 0) {
                        m_va_pltgot_std_string_append = reloc_va;
                        break;
                    }
                }
            }
        }

        if (m_va_pltgot_std_string_append) {
            printf("[*] patch_solution_since<16, 0, 7, 0>: m_va_pltgot_std_string_append = 0x%016lx\n", m_va_pltgot_std_string_append);
        } else {
            printf("[*] patch_solution_since<16, 0, 7, 0>: std::string::append(const char*) is not found.\n");
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
            return false;
        }

        m_tracing = std::move(tracing);

        //
        // get result
        //
        // on AMD64 platform, `std::string` has follow memory layout:
        //     ------------------------------
        //     | offset | size |
        //     ------------------------------
        //     | +0     | 0x8 | char*: raw string ptr
        //     ------------------------------
        //     | +0x8   | 0x8  | size_t: string length
        //     ------------------------------
        //     | +0x10  | 0x8  | `char[16]: a small string buffer` OR `size_t: capacity`
        //     ------------------------------
        //
        uint64_t encoded_key_length;
        x64_emulator.mem_read(init_rdi + 0x8, &encoded_key_length, sizeof(encoded_key_length));
        if (encoded_key_length != official_encoded_key.length()) {
            printf("[-] patch_solution_since<16, 0, 7, 0>: unexpected encoded key length(%lu).\n", encoded_key_length);
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
            return false;
        }

        uint64_t encoded_key_ptr;
        x64_emulator.mem_read(init_rdi, &encoded_key_ptr, sizeof(encoded_key_ptr));

        auto encoded_key = x64_emulator.mem_read(encoded_key_ptr, encoded_key_length);
        if (memcmp(encoded_key.data(), official_encoded_key.data(), encoded_key.size()) == 0) {
            printf("[+] patch_solution_since<16, 0, 7, 0>: official encoded key is found.\n");
            return true;
        } else {
            printf("[-] patch_solution_since<16, 0, 7, 0>: official encoded key is not found.\n");
            printf("[-] patch_solution_since<16, 0, 7, 0>: This patch solution will be suppressed.\n");
            return false;
        }
    }

    bool patch_solution_since<16, 0, 7, 0>::check_rsa_privkey(const rsa_cipher& cipher) {
        return cipher.bits() == 2048;
    }

    void patch_solution_since<16, 0, 7, 0>::make_patch(const rsa_cipher& cipher) {
        auto new_encoded_key = _build_encoded_key(cipher);

        auto CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey =
            m_libcc_interpreter.convert_va_to_ptr<uint8_t*>(m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey);

        std::vector<std::string> patch_code_chunks;
        patch_code_chunks.emplace_back("push rbp;");
        patch_code_chunks.emplace_back("mov rbp, rsp;");
        patch_code_chunks.emplace_back("lea rax, qword ptr [rdi + 0x10];");
        patch_code_chunks.emplace_back("mov qword ptr [rdi], rax;");
        patch_code_chunks.emplace_back("mov qword ptr [rdi + 0x8], 0;");
        {
            std::vector<uint64_t> push_values((new_encoded_key.length() + 1 + 7) / 8, 0);
            memcpy(push_values.data(), new_encoded_key.data(), new_encoded_key.length());

            std::for_each
                (
                    push_values.crbegin(),
                    push_values.crend(),
                    [&patch_code_chunks](uint64_t x) {
                        patch_code_chunks.emplace_back(fmt::format("mov rax, {:#016x};", x));
                        patch_code_chunks.emplace_back("push rax;");
                    }
                );
        }
        patch_code_chunks.emplace_back("mov rsi, rsp;");
        patch_code_chunks.emplace_back(fmt::format("call qword ptr [{:#016x}];", m_va_pltgot_std_string_append));   // keystone will emit `call qword ptr [rip+xxx]` here
        patch_code_chunks.emplace_back("leave;");
        patch_code_chunks.emplace_back("ret;");

        auto x86_assembler = keystone_assembler{ KS_ARCH_X86, KS_MODE_64 };
        auto reliable_areas = _calculate_reliable_areas();

        auto current_va = m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey;
        auto current_ptr = m_libcc_interpreter.convert_va_to_ptr<uint8_t*>(current_va);

        auto current_reliable_area = reliable_areas.begin();
        auto current_reliable_area_begin_va = current_reliable_area->first;
        auto current_reliable_area_end_va = current_reliable_area_begin_va + current_reliable_area->second;

        for (const auto& patch_code_chunk : patch_code_chunks) {
            while (true) { 
                auto assembled_patch_code_chunk = x86_assembler.assemble(patch_code_chunk, current_va);
                constexpr size_t jmp_code_max_size = 1 + sizeof(uint32_t);

                if (current_reliable_area_begin_va <= current_va && current_va + assembled_patch_code_chunk.size() + jmp_code_max_size <= current_reliable_area_end_va) {
                    memcpy(current_ptr, assembled_patch_code_chunk.data(), assembled_patch_code_chunk.size());
                    current_va += assembled_patch_code_chunk.size();
                    current_ptr += assembled_patch_code_chunk.size();
                    break;
                } else {
                    auto next_reliable_area = std::next(current_reliable_area);
                    while (next_reliable_area->second < 0x10) {
                        next_reliable_area = std::next(next_reliable_area);
                    }

                    auto assembled_jmp = x86_assembler.assemble(fmt::format("jmp {:#016x};", next_reliable_area->first), current_va);

                    memcpy(current_ptr, assembled_jmp.data(), assembled_jmp.size());
                    current_va = next_reliable_area->first;
                    current_ptr = m_libcc_interpreter.convert_va_to_ptr<uint8_t*>(next_reliable_area->first);

                    current_reliable_area = next_reliable_area;
                    current_reliable_area_begin_va = current_reliable_area->first;
                    current_reliable_area_end_va = current_reliable_area_begin_va + current_reliable_area->second;
                }
            }
        }

        printf("[*] patch_solution_since<16, 0, 7, 0>: Patch has been done.\n");
    }
}
