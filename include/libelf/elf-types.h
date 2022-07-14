/* libelf - A portable C library for manage ELF files
    Copyright (C) <2022>  <Gabriel Correia>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef LIBELF_ELFTYPES_H
#define LIBELF_ELFTYPES_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Sizeof the ELF indentifer buffer specified in the 'ELF' format documentation */
#define EI_NIDENT 16

typedef struct
{ 
    uint8_t  elf_ident[EI_NIDENT];
    uint16_t elf_type;
    uint16_t elf_machine;
    uint32_t elf_version;
    uint32_t elf_entry;
    uint32_t elf_phoff;
    uint32_t elf_shoff;
    uint32_t elf_flags;
    uint16_t elf_ehsize;
    uint16_t elf_phentsize;
    uint16_t elf_phnum;
    uint16_t elf_shentsize;
    uint16_t elf_shnum;
    uint16_t elf_shstrndx;
} elf_header32_t;

typedef struct
{
    /* Buffer used for interpreter the file with his magic and some others
     * usefull components.
    */
    uint8_t  elf_ident[EI_NIDENT];
    uint16_t elf_type;
    uint16_t elf_machine;
    uint32_t elf_version;
    uint64_t elf_entry;
    uint64_t elf_phoff;
    uint64_t elf_shoff;
    uint32_t elf_flags;
    uint16_t elf_ehsize;
    uint16_t elf_phentsize;
    uint16_t elf_phnum;
    uint16_t elf_shentsize;
    uint16_t elf_shnum;
    uint16_t elf_shstrndx;
} elf_header64_t;


typedef enum
{
    ELF_E_OK, 
    ELF_E_LOAD_FILE, 
    ELF_E_UNLOAD_FILE, 
    ELF_E_CANT_READ,
    ELF_E_INVALID_ELF,
    ELF_E_INVALID_ARCH,
    ELF_E_UNKNOW_CPU_ENCODE,
    ELF_E_INVALID_VERSION,
    ELF_E_INVALID_TYPE_NAME,
    ELF_E_ERROR_CODE,
    ELF_E_NULL_VALUE
} elf_err_e;

typedef enum
{
    EA_INVALID,
    EA_32BITS,
    EA_64BITS,
    EA_NULL_VALUE
} elf_arch_e;

typedef enum
{
    ECE_UNKNOW,
    /* little-endian */
    ECE_LSB_2DATA,
    /* big-endian */
    ECE_MSB_2DATA,
    ECE_NULL_VALUE
} elf_cpu_encode_e;

typedef enum
{
    EV_INVALID,
    EV_CURRENT,
    EV_NULL_VALUE
} elf_version_e;

typedef struct
{
    /* ELF filename (Virtual position in the file system) */
    const char *elf_name;
    /* ELF FILE pointer (A pointer for the current ELF archive) */
    FILE *elf_file_ptr;

    const char *error_record;

    /* Independent of the type of the ELF file, the first readed chunk 
     * of the file must coverage some others members of this structure.
     * After decide what version the current ELF is, the correct parser 
     * will be performed. 
    */
    elf_header64_t elf;

    /* Avoid multiples casting type */
    elf_header64_t *elf_header64;
    elf_header32_t *elf_header32;

    /* ELF architecture type */
    elf_arch_e arch_type;
    elf_cpu_encode_e cpu_encode;
    elf_version_e version;

    elf_err_e error_id;

} elf_ctx_t;


#endif
