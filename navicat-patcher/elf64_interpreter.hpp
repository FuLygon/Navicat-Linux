#pragma once
#include <stddef.h>
#include <stdint.h>
#include <elf.h>

#include <map>
#include <string>
#include <optional>

#include "exception.hpp"
#include "memory_utility.hpp"

namespace nkg {

    class elf64_interpreter {
    public:
        using fo_t = uintptr_t;
        using rva_t = uintptr_t;
        using va_t = uintptr_t;

        class parse_error;
        class bad_fo_exception;
        class bad_va_exception;

    private:
        size_t m_elf_size;
        Elf64_Ehdr* m_elf_header;
        Elf64_Phdr* m_elf_program_headers;
        Elf64_Shdr* m_elf_section_headers;

        std::map<Elf64_Addr, Elf64_Phdr*> m_segment_va_lookup_table;
        std::map<Elf64_Off, Elf64_Phdr*> m_segment_fo_lookup_table;

        std::map<Elf64_Addr, Elf64_Shdr*> m_section_va_lookup_table;
        std::map<Elf64_Off, Elf64_Shdr*> m_section_fo_lookup_table;
        std::map<std::string_view, Elf64_Shdr*> m_section_name_lookup_table;

        Elf64_Dyn* m_dynamic_rela;
        Elf64_Dyn* m_dynamic_relasz;
        Elf64_Dyn* m_dynamic_rel;
        Elf64_Dyn* m_dynamic_relsz;
        Elf64_Dyn* m_dynamic_pltgot;
        Elf64_Dyn* m_dynamic_jmprel;
        Elf64_Dyn* m_dynamic_pltrel;
        Elf64_Dyn* m_dynamic_pltrelsz;
        Elf64_Dyn* m_dynamic_symtab;
        Elf64_Dyn* m_dynamic_strtab;

        std::map<va_t, size_t> m_relocation_distribute;

        elf64_interpreter();

    public:
        [[nodiscard]]
        static elf64_interpreter parse(void* image_ptr, size_t image_size);

        [[nodiscard]]
        size_t elf_size() const noexcept;

        template<typename return_t = void*>
        [[nodiscard]]
        return_t elf_base() const noexcept {
            static_assert(std::is_pointer_v<return_t>);
            return reinterpret_cast<return_t>(m_elf_header);
        }

        template<typename return_t = void*>
        [[nodiscard]]
        return_t elf_offset(fo_t offset) const noexcept {
            static_assert(std::is_pointer_v<return_t>);
            return address_offset_cast<return_t>(m_elf_header, offset);
        }

        [[nodiscard]]
        Elf64_Ehdr* elf_header() const noexcept;

        [[nodiscard]]
        Elf64_Phdr* elf_program_header(size_t n) const;

        [[nodiscard]]
        Elf64_Phdr* elf_program_header_from_fo(fo_t file_offset) const;

        [[nodiscard]]
        Elf64_Phdr* elf_program_header_from_rva(rva_t rva) const;

        [[nodiscard]]
        Elf64_Phdr* elf_program_header_from_va(va_t va) const;

        [[nodiscard]]
        size_t elf_program_headers_num() const noexcept;

        [[nodiscard]]
        Elf64_Shdr* elf_section_header(size_t n) const;

        [[nodiscard]]
        Elf64_Shdr* elf_section_header(std::string_view section_name) const;

        [[nodiscard]]
        size_t elf_section_headers_num() const noexcept;

        template<typename return_t = void*>
        [[nodiscard]]
        return_t elf_section_view(size_t n, fo_t offset = 0) const {
            return elf_offset<return_t>(elf_section_header(n)->sh_offset + offset);
        }

        template<typename return_t = void*>
        [[nodiscard]]
        return_t elf_section_view(std::string_view section_name, fo_t offset = 0) const {
            return elf_offset<return_t>(elf_section_header(section_name)->sh_offset + offset);
        }

        template<typename ptr_t>
        [[nodiscard]]
        fo_t convert_ptr_to_fo(ptr_t ptr) const {
            return address_delta(ptr, m_elf_header);
        }

        [[nodiscard]]
        fo_t convert_rva_to_fo(rva_t rva) const;

        [[nodiscard]]
        fo_t convert_va_to_fo(va_t va) const;

        [[nodiscard]]
        rva_t convert_fo_to_rva(fo_t file_offset) const;

        template<typename ptr_t>
        [[nodiscard]]
        rva_t convert_ptr_to_rva(ptr_t ptr) const {
            return convert_fo_to_rva(convert_ptr_to_fo(ptr));
        }

        rva_t convert_va_to_rva(va_t va) const;

        [[nodiscard]]
        va_t convert_fo_to_va(fo_t file_offset) const;

        [[nodiscard]]
        va_t convert_rva_to_va(rva_t rva) const;

        template<typename ptr_t>
        va_t convert_ptr_to_va(ptr_t ptr) const {
            return convert_fo_to_va(convert_ptr_to_fo(ptr));
        }

        template<typename ptr_t = void*>
        [[nodiscard]]
        ptr_t convert_fo_to_ptr(fo_t offset) const {
            return elf_offset<ptr_t>(offset);
        }

        template<typename ptr_t = void*>
        [[nodiscard]]
        ptr_t convert_rva_to_ptr(rva_t rva) const {
            return convert_fo_to_ptr<ptr_t>(convert_rva_to_fo(rva));
        }

        template<typename ptr_t = void*>
        [[nodiscard]]
        ptr_t convert_va_to_ptr(va_t va) const {
            return convert_fo_to_ptr<ptr_t>(convert_va_to_fo(va));
        }

        template<typename return_t = void*, typename callable_t>
        [[nodiscard]]
        return_t search_section(size_t n, callable_t&& pred_func) const noexcept {
            auto sect_hdr = elf_section_header(n);
            auto sect_view = elf_offset<uint8_t*>(sect_hdr->sh_offset);
            for (size_t i = 0; i < sect_hdr->sh_size; ++i) {
                if (pred_func(sect_view, i, sect_hdr->sh_size)) {
                    return reinterpret_cast<return_t>(sect_view + i);
                }
            }
            return nullptr;
        }

        template<typename return_t = void*, typename callable_t>
        [[nodiscard]]
        return_t search_section(std::string_view section_name, callable_t&& pred_func) const noexcept {
            auto sect_hdr = elf_section_header(section_name);
            auto sect_view = elf_offset<uint8_t*>(sect_hdr->sh_offset);
            for (size_t i = 0; i < sect_hdr->sh_size; ++i) {
                if (pred_func(sect_view, i, sect_hdr->sh_size)) {
                    return reinterpret_cast<return_t>(sect_view + i);
                }
            }
            return nullptr;
        }

        [[nodiscard]]
        std::optional<va_t> elf_dynamic_rela() const;

        [[nodiscard]]
        std::optional<size_t> elf_dynamic_relasz() const;

        [[nodiscard]]
        std::optional<va_t> elf_dynamic_rel() const;

        [[nodiscard]]
        std::optional<size_t> elf_dynamic_relsz() const;

        [[nodiscard]]
        std::optional<va_t> elf_dynamic_pltgot() const;

        [[nodiscard]]
        std::optional<va_t> elf_dynamic_jmprel() const;

        [[nodiscard]]
        std::optional<int> elf_dynamic_pltrel() const;

        [[nodiscard]]
        std::optional<size_t> elf_dynamic_pltrelsz() const;

        [[nodiscard]]
        std::optional<va_t> elf_dynamic_symtab() const;

        [[nodiscard]]
        std::optional<va_t> elf_dynamic_strtab() const;

        [[nodiscard]]
        const std::map<va_t, size_t>& relocation_distribute() const;
    };

    class elf64_interpreter::parse_error : public ::nkg::exception {
        using ::nkg::exception::exception;
    };

    class elf64_interpreter::bad_fo_exception : public ::nkg::exception {
        using ::nkg::exception::exception;
    };

    class elf64_interpreter::bad_va_exception : public ::nkg::exception {
        using ::nkg::exception::exception;
    };

}
