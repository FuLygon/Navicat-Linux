#include "elf64_interpreter.hpp"
#include "exceptions/index_exception.hpp"
#include "exceptions/key_exception.hpp"
#include <memory.h>
#include <algorithm>
#include <fmt/format.h>

#define NKG_CURRENT_SOURCE_FILE() ".\\navicat-patcher\\elf64_interpreter.cpp"
#define NKG_CURRENT_SOURCE_LINE() __LINE__

namespace nkg {

    elf64_interpreter::elf64_interpreter() :
        m_elf_size(0),
        m_elf_header(nullptr),
        m_elf_program_headers(nullptr),
        m_elf_section_headers(nullptr),
        m_dynamic_rela(nullptr),
        m_dynamic_relasz(nullptr),
        m_dynamic_rel(nullptr),
        m_dynamic_relsz(nullptr),
        m_dynamic_pltgot(nullptr),
        m_dynamic_jmprel(nullptr),
        m_dynamic_pltrel(nullptr),
        m_dynamic_pltrelsz(nullptr),
        m_dynamic_symtab(nullptr),
        m_dynamic_strtab(nullptr) {}

    [[nodiscard]]
    elf64_interpreter elf64_interpreter::parse(void* image_ptr, size_t image_size) {
        elf64_interpreter new_image;

        // check ELF header
        new_image.m_elf_size = image_size;
        new_image.m_elf_header = reinterpret_cast<Elf64_Ehdr*>(image_ptr);
        if (is_address_in_range(new_image.m_elf_header, sizeof(Elf64_Ehdr), image_ptr, image_size) == false) {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: image is corrupted.");
        }

        if (memcmp(new_image.m_elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: header magic check failed.");
        }

        if (new_image.m_elf_header->e_ident[EI_CLASS] != ELFCLASS64) {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Unsupported ELF file: not ELF64 image.");
        }

        if (new_image.m_elf_header->e_ident[EI_DATA] == ELFDATA2LSB && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) {
            ; // pass
        } else if (new_image.m_elf_header->e_ident[EI_DATA] == ELFDATA2MSB && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) {
            ; // pass
        } else {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Unsupported ELF file: unsupported endian.");
        }

        if (new_image.m_elf_header->e_ident[EI_VERSION] != EV_CURRENT) {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_ident[EI_VERSION] check failed.");
        }

        // new_image.m_elf_header->e_ident[EI_OSABI]
        // new_image.m_elf_header->e_ident[EI_ABIVERSION]

        for (int i = EI_PAD; i < sizeof(new_image.m_elf_header->e_ident); ++i) {
            if (new_image.m_elf_header->e_ident[i] != 0) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_ident padding contains non-zero byte(s).");
            }
        }

        if (new_image.m_elf_header->e_version != EV_CURRENT) {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_version check failed.");
        }

        if (new_image.m_elf_header->e_ehsize != sizeof(Elf64_Ehdr)) {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_ehsize check failed.");
        }

        if (new_image.m_elf_header->e_phoff && new_image.m_elf_header->e_phentsize && new_image.m_elf_header->e_phnum) {
            if (new_image.m_elf_header->e_phentsize != sizeof(Elf64_Phdr)) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_phentsize check failed.");
            }

            new_image.m_elf_program_headers = address_offset_cast<Elf64_Phdr*>(image_ptr, new_image.m_elf_header->e_phoff);

            if (is_address_in_range(new_image.m_elf_program_headers, new_image.m_elf_header->e_phnum * sizeof(Elf64_Phdr), image_ptr, image_size) == false) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: image is corrupted.");
            }
        } else if (new_image.m_elf_header->e_phoff == 0 && new_image.m_elf_header->e_phentsize == 0 && new_image.m_elf_header->e_phnum == 0) {
            new_image.m_elf_program_headers = nullptr;
        } else {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_ph* check failed.");
        }

        if (new_image.m_elf_header->e_shoff && new_image.m_elf_header->e_shentsize && new_image.m_elf_header->e_shnum) {
            if (new_image.m_elf_header->e_shentsize != sizeof(Elf64_Shdr)) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_shentsize check failed.");
            }

            new_image.m_elf_section_headers = address_offset_cast<Elf64_Shdr*>(image_ptr, new_image.m_elf_header->e_shoff);

            if (is_address_in_range(new_image.m_elf_section_headers, new_image.m_elf_header->e_shnum * sizeof(Elf64_Shdr), image_ptr, image_size) == false) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: image is corrupted.");
            }
        } else if (new_image.m_elf_header->e_shoff == 0 && new_image.m_elf_header->e_shentsize == 0 && new_image.m_elf_header->e_shnum == 0) {
            new_image.m_elf_section_headers = nullptr;
        } else {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_sh* check failed.");
        }

        if (new_image.m_elf_header->e_shstrndx != SHN_UNDEF) {
            if (new_image.m_elf_header->e_shstrndx >= new_image.m_elf_header->e_shnum) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: Elf64_Ehdr::e_shstrndx is out of range.");
            }
        }

        // check program header table and section header table are not overlapped
        if (new_image.m_elf_program_headers && new_image.m_elf_section_headers) {
            auto a1 = new_image.m_elf_program_headers;
            auto a2 = new_image.m_elf_program_headers + new_image.m_elf_header->e_phnum;
            auto b1 = new_image.m_elf_section_headers;
            auto b2 = new_image.m_elf_section_headers + new_image.m_elf_header->e_shnum;
            bool not_overlapped = address_delta(a1, b1) < 0 && address_delta(a2, b1) <= 0 || address_delta(b1, a1) < 0 && address_delta(b2, a1) <= 0;
            if (!not_overlapped) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: program header table and section header table overlapped.");
            }
        }

        // parse program header
        for (size_t i = 0; i < new_image.m_elf_header->e_phnum; ++i) {
            auto& prog_hdr = new_image.m_elf_program_headers[i];
            if (!is_address_in_range(address_offset(image_ptr, static_cast<ptrdiff_t>(prog_hdr.p_offset)), prog_hdr.p_filesz, image_ptr, image_size)) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: image is corrupted.");
            }

            auto prog_hdr_align = prog_hdr.p_align;
            if (prog_hdr_align) {
                // align must be a power of 2
                if ((prog_hdr_align & (prog_hdr_align - 1)) != 0) {
                    throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Phdr[{}]: p_align is not a power of 2.", i));
                }

                if (prog_hdr.p_offset % prog_hdr_align != prog_hdr.p_vaddr % prog_hdr_align) {
                    throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Phdr[{}]: p_offset !== p_vaddr (mod prog_hdr_align).", i));
                }
            }

            if (prog_hdr.p_type == PT_LOAD) {
                new_image.m_segment_va_lookup_table.emplace(std::make_pair(prog_hdr.p_vaddr, &prog_hdr));
                new_image.m_segment_fo_lookup_table.emplace(std::make_pair(prog_hdr.p_offset, &prog_hdr));
            }
        }

        // parse section header
        if (new_image.m_elf_header->e_shstrndx != SHN_UNDEF) {
            auto sect_hdr_strtab = &new_image.m_elf_section_headers[new_image.m_elf_header->e_shstrndx];
            auto sect_view_strtab = address_offset_cast<const char*>(image_ptr, sect_hdr_strtab->sh_offset);

            if (sect_hdr_strtab->sh_type != SHT_STRTAB) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: sect_hdr_strtab->sh_type != SHT_STRTAB.");
            }

            if (!is_address_in_range(sect_view_strtab, sect_hdr_strtab->sh_size, image_ptr, image_size)) {
                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: image is corrupted.");
            }

            for (size_t i = 0; i < new_image.m_elf_header->e_shnum; ++i) {
                new_image.m_section_name_lookup_table
                    .emplace(std::make_pair(std::string_view(address_offset(sect_view_strtab, new_image.m_elf_section_headers[i].sh_name)), &new_image.m_elf_section_headers[i]));
            }
        }

        for (int i = 0; i < new_image.m_elf_header->e_shnum; ++i) {
            auto& sect_hdr = new_image.m_elf_section_headers[i];
            switch (sect_hdr.sh_type) {
                case SHT_SYMTAB:
                    if (sect_hdr.sh_entsize != sizeof(Elf64_Sym)) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_entsize != sizeof(Elf64_Sym).", i));
                    }

                    // check sh_link
                    if (sect_hdr.sh_link == SHN_UNDEF) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link == SHN_UNDEF.", i));
                    }

                    if (sect_hdr.sh_link >= new_image.m_elf_header->e_shnum) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link is out of range.", i));
                    }

                    if (new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_STRTAB) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: bad value of sh_link.", i));
                    }

                    // todo: check sh_info
                    break;
                case SHT_RELA:
                    if (sect_hdr.sh_entsize != sizeof(Elf64_Rela)) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_entsize != sizeof(Elf64_Rela).", i));
                    }

                    // check sh_link
                    if (sect_hdr.sh_link == SHN_UNDEF) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link == SHN_UNDEF.", i));
                    }

                    if (sect_hdr.sh_link >= new_image.m_elf_header->e_shnum) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link is out of range.", i));
                    }

                    if (new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_SYMTAB && new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_DYNSYM) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[%u]: bad value of sh_link.", i));
                    }

                    // check sh_info
                    if (sect_hdr.sh_flags & SHF_INFO_LINK) {
                        if (sect_hdr.sh_info == SHN_UNDEF) {
                            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info == SHN_UNDEF.", i));
                        }

                        if (sect_hdr.sh_info >= new_image.m_elf_header->e_shnum) {
                            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info is out of range.", i));
                        }
                    } else {
                        if (sect_hdr.sh_info != 0) {
                            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info != 0.", i));
                        }
                    }
                    break;
                case SHT_HASH:
                    if (sect_hdr.sh_link == SHN_UNDEF) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link == SHN_UNDEF.", i));
                    }

                    if (sect_hdr.sh_link >= new_image.m_elf_header->e_shnum) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link is out of range.", i));
                    }

                    if (new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_SYMTAB && new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_DYNSYM) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: bad value of sh_link.", i));
                    }

                    if (sect_hdr.sh_info != 0) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info != 0.", i));
                    }
                    break;
                case SHT_DYNAMIC:
                    if (sect_hdr.sh_entsize != sizeof(Elf64_Dyn)) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_entsize != sizeof(Elf64_Dyn).", i));
                    }

                    // check sh_link
                    if (sect_hdr.sh_link == SHN_UNDEF) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link == SHN_UNDEF.", i));
                    }

                    if (sect_hdr.sh_link >= new_image.m_elf_header->e_shnum) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link is out of range.", i));
                    }

                    if (new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_STRTAB) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: bad value of sh_link.", i));
                    }

                    // check sh_info
                    if (sect_hdr.sh_info != 0) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info != 0.", i));
                    }
                    break;
                case SHT_REL:
                    if (sect_hdr.sh_entsize != sizeof(Elf64_Rel)) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_entsize != sizeof(Elf64_Rel).", i));
                    }

                    // check sh_link
                    if (sect_hdr.sh_link == SHN_UNDEF) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link == SHN_UNDEF.", i));
                    }

                    if (sect_hdr.sh_link >= new_image.m_elf_header->e_shnum) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link is out of range.", i));
                    }

                    if (new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_SYMTAB && new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_DYNSYM) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[%u]: bad value of sh_link.", i));
                    }

                    // check sh_info
                    if (sect_hdr.sh_flags & SHF_INFO_LINK) {
                        if (sect_hdr.sh_info == SHN_UNDEF) {
                            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info == SHN_UNDEF.", i));
                        }

                        if (sect_hdr.sh_info >= new_image.m_elf_header->e_shnum) {
                            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info is out of range.", i));
                        }
                    } else {
                        if (sect_hdr.sh_info != 0) {
                            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_info != 0.", i));
                        }
                    }
                    break;
                case SHT_DYNSYM:
                    if (sect_hdr.sh_entsize != sizeof(Elf64_Sym)) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_entsize != sizeof(Elf64_Dyn).", i));
                    }

                    // check sh_link
                    if (sect_hdr.sh_link == SHN_UNDEF) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link == SHN_UNDEF.", i));
                    }

                    if (sect_hdr.sh_link >= new_image.m_elf_header->e_shnum) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_link is out of range.", i));
                    }

                    if (new_image.m_elf_section_headers[sect_hdr.sh_link].sh_type != SHT_STRTAB) {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: bad value of sh_link.", i));
                    }

                    // todo: check sh_info
                    break;
                default:
                    break;
            }

            if (sect_hdr.sh_type != SHT_NOBITS) {
                if (is_address_in_range(address_offset(image_ptr, sect_hdr.sh_offset), sect_hdr.sh_size, image_ptr, image_size) == false) {
                    throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: image is corrupted.", i));
                }
                new_image.m_section_fo_lookup_table.emplace(std::make_pair(sect_hdr.sh_offset, new_image.m_elf_section_headers + i));
            }

            if (sect_hdr.sh_addr) {
                if (sect_hdr.sh_addralign && sect_hdr.sh_addr % sect_hdr.sh_addralign != 0) {
                    throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: Elf64_Shdr[{}]: sh_addr is not aligned to sh_addralign.", i));
                }
                new_image.m_section_va_lookup_table.emplace(std::make_pair(sect_hdr.sh_addr, &new_image.m_elf_section_headers[i]));
            }
        }

        // parse program header, second parse
        for (size_t i = 0; i < new_image.m_elf_header->e_phnum; ++i) {
            auto& prog_hdr = new_image.m_elf_program_headers[i];
            if (prog_hdr.p_type == PT_DYNAMIC) {
                auto seg_dynamic_base = address_offset_cast<Elf64_Dyn*>(image_ptr, prog_hdr.p_offset);
                auto seg_dyncmic_size = prog_hdr.p_filesz;

                // first parse
                for (size_t j = 0; j * sizeof(Elf64_Dyn) < seg_dyncmic_size && seg_dynamic_base[j].d_tag != DT_NULL; ++j) {
                    auto& dyn_entry = seg_dynamic_base[j];
                    switch (dyn_entry.d_tag) {
                        case DT_RELA:
                            new_image.m_dynamic_rela = &seg_dynamic_base[j];
                            break;
                        case DT_RELAENT:
                            if (seg_dynamic_base[j].d_un.d_val != sizeof(Elf64_Rela)) {
                                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: the value of DT_RELAENT.dval != sizeof(Elf64_Rela).");
                            }
                            break;
                        case DT_RELASZ:
                            new_image.m_dynamic_relasz = &seg_dynamic_base[j];
                            break;
                        case DT_REL:
                            new_image.m_dynamic_rel = &seg_dynamic_base[j];
                            break;
                        case DT_RELENT:
                            if (seg_dynamic_base[j].d_un.d_val != sizeof(Elf64_Rel)) {
                                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: the value of DT_RELENT.dval != sizeof(Elf64_Rel).");
                            }
                            break;
                        case DT_RELSZ:
                            new_image.m_dynamic_relsz = &seg_dynamic_base[j];
                            break;
                        case DT_PLTGOT:
                            new_image.m_dynamic_pltgot = &seg_dynamic_base[j];
                            break;
                        case DT_JMPREL:
                            new_image.m_dynamic_jmprel = &seg_dynamic_base[j];
                            break;
                        case DT_PLTREL:
                            if (seg_dynamic_base[j].d_un.d_val == DT_REL || seg_dynamic_base[j].d_un.d_val == DT_RELA) {
                                new_image.m_dynamic_pltrel = &seg_dynamic_base[j];
                            } else {
                                throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Bad ELF file: the value of DT_PLTREL.dval is neither DT_REL nor DT_RELA.");
                            }
                            break;
                        case DT_PLTRELSZ:
                            new_image.m_dynamic_pltrelsz = &seg_dynamic_base[j];
                            break;
                        case DT_STRTAB:
                            new_image.m_dynamic_strtab = &seg_dynamic_base[j];
                            break;
                        case DT_SYMTAB:
                            new_image.m_dynamic_symtab = &seg_dynamic_base[j];
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        if (new_image.m_dynamic_rela && new_image.m_dynamic_relasz) {
            auto rela_base = new_image.convert_va_to_ptr<Elf64_Rela*>(new_image.m_dynamic_rela->d_un.d_ptr);
            auto rela_size = new_image.m_dynamic_relasz->d_un.d_val;
            for (size_t i = 0; i * sizeof(Elf64_Rela) < rela_size; ++i) {
                auto reloc_va = rela_base[i].r_offset;
                auto reloc_type = ELF64_R_TYPE(rela_base[i].r_info);
                switch(reloc_type) {
                    case R_X86_64_64:
                    case R_X86_64_GLOB_DAT:
                    case R_X86_64_RELATIVE:
                        new_image.m_relocation_distribute.emplace(std::make_pair(reloc_va, 8));
                        break;
                    default:
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Unsupported ELF file: unhandled relocation type({}).", reloc_type));
                }
            }
        }

        if (new_image.m_dynamic_rel && new_image.m_dynamic_relsz) {
            throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Unsupported ELF file: DT_REL is not parsed.");
        }

        if (new_image.m_dynamic_jmprel && new_image.m_dynamic_pltrel && new_image.m_dynamic_pltrelsz) {
            if (new_image.m_dynamic_pltrel->d_un.d_val == DT_RELA) {
                auto jmprel_base = new_image.convert_va_to_ptr<Elf64_Rela*>(new_image.m_dynamic_jmprel->d_un.d_ptr);
                auto jmprel_size = new_image.m_dynamic_pltrelsz->d_un.d_val;
                for (size_t i = 0; i * sizeof(Elf64_Rela) < jmprel_size; ++i) {
                    auto reloc_va = jmprel_base[i].r_offset;
                    auto reloc_type = ELF64_R_TYPE(jmprel_base[i].r_info);
                    if (reloc_type == R_X86_64_JUMP_SLOT) {
                        new_image.m_relocation_distribute.emplace(std::make_pair(reloc_va, 8));
                    } else {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: bad relocation type({}) in JMPREL relocation table.", reloc_type));
                    }
                }
            } else {
                auto jmprel_base = new_image.convert_va_to_ptr<Elf64_Rel*>(new_image.m_dynamic_jmprel->d_un.d_ptr);
                auto jmprel_size = new_image.m_dynamic_pltrelsz->d_un.d_val;
                for (size_t i = 0; i * sizeof(Elf64_Rela) < jmprel_size; ++i) {
                    auto reloc_va = jmprel_base[i].r_offset;
                    auto reloc_type = ELF64_R_TYPE(jmprel_base[i].r_info);
                    if (reloc_type == R_X86_64_JUMP_SLOT) {
                        new_image.m_relocation_distribute.emplace(std::make_pair(reloc_va, 8));
                    } else {
                        throw parse_error(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Bad ELF file: bad relocation type({}) in JMPREL relocation table.", reloc_type));
                    }
                }
            }
        }

        return new_image;
    }

    [[nodiscard]]
    size_t elf64_interpreter::elf_size() const noexcept {
        return m_elf_size;
    }

    [[nodiscard]]
    Elf64_Ehdr* elf64_interpreter::elf_header() const noexcept {
        return m_elf_header;
    }

    [[nodiscard]]
    Elf64_Phdr* elf64_interpreter::elf_program_header(size_t n) const {
        if (n < m_elf_header->e_phnum) {
            return m_elf_program_headers + n;
        } else {
            throw exceptions::index_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Out of range.");
        }
    }

    [[nodiscard]]
    Elf64_Phdr* elf64_interpreter::elf_program_header_from_fo(fo_t file_offset) const {
        auto it = m_segment_fo_lookup_table.upper_bound(file_offset);
        if (it != m_segment_fo_lookup_table.begin()) {
            --it;
            if (it->second->p_offset <= file_offset && file_offset < it->second->p_offset + it->second->p_filesz) {
                return it->second;
            }
        }
        throw bad_fo_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("File offset({:#x}) doesn't point to any segment."));
    }

    [[nodiscard]]
    Elf64_Phdr* elf64_interpreter::elf_program_header_from_rva(rva_t rva) const {
        return elf_program_header_from_va(convert_rva_to_va(rva));
    }

    [[nodiscard]]
    Elf64_Phdr* elf64_interpreter::elf_program_header_from_va(va_t va) const {
        auto it = m_segment_va_lookup_table.upper_bound(va);
        if (it != m_segment_va_lookup_table.begin()) {
            --it;
            if (it->second->p_vaddr <= va && va < it->second->p_vaddr + it->second->p_memsz) {
                return it->second;
            }
        }
        throw bad_va_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Invalid virtual address({:#016x}).", va));
    }

    [[nodiscard]]
    size_t elf64_interpreter::elf_program_headers_num() const noexcept {
        return m_elf_header->e_shnum;
    }

    [[nodiscard]]
    Elf64_Shdr* elf64_interpreter::elf_section_header(size_t n) const {
        if (n < m_elf_header->e_shnum) {
            return m_elf_section_headers + n;
        } else {
            throw exceptions::index_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), "Out of range.");
        }
    }

    [[nodiscard]]
    Elf64_Shdr* elf64_interpreter::elf_section_header(std::string_view section_name) const {
        auto it = m_section_name_lookup_table.find(section_name);
        if (it != m_section_name_lookup_table.end()) {
            return it->second;
        } else {
            throw exceptions::key_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Section `{}` is not found.", section_name.data()));
        }
    }

    [[nodiscard]]
    size_t elf64_interpreter::elf_section_headers_num() const noexcept {
        return m_elf_header->e_shnum;
    }

    [[nodiscard]]
    elf64_interpreter::fo_t elf64_interpreter::convert_rva_to_fo(rva_t rva) const {
        return convert_va_to_fo(convert_rva_to_va(rva));
    }

    [[nodiscard]]
    elf64_interpreter::fo_t elf64_interpreter::convert_va_to_fo(va_t va) const {
        auto it = m_segment_va_lookup_table.upper_bound(va);
        if (it != m_segment_va_lookup_table.begin()) {
            --it;
            if (it->second->p_vaddr <= va && va < it->second->p_vaddr + it->second->p_memsz) {
                if (va - it->second->p_vaddr < it->second->p_filesz) {
                    return it->second->p_offset + (va - it->second->p_vaddr);
                } else {
                    throw bad_va_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Virtual address({:#016x}) doesn't have corresponding file offset.", va));
                }
            }
        }
        throw bad_va_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("Invalid virtual address({:#016x})", va));
    }

    [[nodiscard]]
    elf64_interpreter::rva_t elf64_interpreter::convert_fo_to_rva(fo_t file_offset) const {
        return convert_va_to_rva(convert_fo_to_va(file_offset));
    }

    elf64_interpreter::rva_t elf64_interpreter::convert_va_to_rva(va_t va) const {
        return va - m_segment_va_lookup_table.begin()->first;
    }

    [[nodiscard]]
    elf64_interpreter::va_t elf64_interpreter::convert_fo_to_va(fo_t file_offset) const {
        auto it = m_segment_fo_lookup_table.upper_bound(file_offset);
        if (it != m_segment_fo_lookup_table.begin()) {
            --it;
            if (it->second->p_offset <= file_offset && file_offset < it->second->p_offset + it->second->p_filesz) {
                return it->second->p_vaddr + (file_offset - it->second->p_offset);
            }
        }
        throw bad_fo_exception(NKG_CURRENT_SOURCE_FILE(), NKG_CURRENT_SOURCE_LINE(), fmt::format("File offset({:#x}) doesn't have corresponding virtual address.", file_offset));
    }

    [[nodiscard]]
    elf64_interpreter::va_t elf64_interpreter::convert_rva_to_va(rva_t rva) const {
        return m_segment_va_lookup_table.begin()->first + rva;
    }

    [[nodiscard]]
    std::optional<elf64_interpreter::va_t> elf64_interpreter::elf_dynamic_rela() const {
        return m_dynamic_rela ? std::make_optional(m_dynamic_rela->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<size_t> elf64_interpreter::elf_dynamic_relasz() const {
        return m_dynamic_relasz ? std::make_optional(m_dynamic_relasz->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<elf64_interpreter::va_t> elf64_interpreter::elf_dynamic_rel() const {
        return m_dynamic_rel ? std::make_optional(m_dynamic_rel->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<size_t> elf64_interpreter::elf_dynamic_relsz() const {
        return m_dynamic_relsz ? std::make_optional(m_dynamic_relsz->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<elf64_interpreter::va_t> elf64_interpreter::elf_dynamic_pltgot() const {
        return m_dynamic_pltgot ? std::make_optional(m_dynamic_pltgot->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<elf64_interpreter::va_t> elf64_interpreter::elf_dynamic_jmprel() const {
        return m_dynamic_jmprel ? std::make_optional(m_dynamic_jmprel->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<int> elf64_interpreter::elf_dynamic_pltrel() const {
        return m_dynamic_pltrel ? std::make_optional(m_dynamic_pltrel->d_un.d_val) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<size_t> elf64_interpreter::elf_dynamic_pltrelsz() const {
        return m_dynamic_pltrelsz ? std::make_optional(m_dynamic_pltrelsz->d_un.d_val) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<elf64_interpreter::va_t> elf64_interpreter::elf_dynamic_symtab() const {
        return m_dynamic_symtab ? std::make_optional(m_dynamic_symtab->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    std::optional<elf64_interpreter::va_t> elf64_interpreter::elf_dynamic_strtab() const {
        return m_dynamic_strtab ? std::make_optional(m_dynamic_strtab->d_un.d_ptr) : std::nullopt;
    }

    [[nodiscard]]
    const std::map<elf64_interpreter::va_t, size_t>& elf64_interpreter::relocation_distribute() const {
        return m_relocation_distribute;
    }
}

#undef NKG_CURRENT_SOURCE_LINE
#undef NKG_CURRENT_SOURCE_FILE
