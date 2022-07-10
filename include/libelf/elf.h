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

#ifndef LIBELF_ELF_H
#define LIBELF_ELF_H 

typedef struct
{
    /* ELF filename (Virtual position in the file system) */
    const char *elf_name;
    /* ELF FILE pointer (A pointer for the current attached ELF archive) */
    FILE *elf_file_ptr;

    const char *error_record;

} elf_ctx_t;

typedef enum elf_err_e
{
    ELF_OK, LOAD_FILE_ERROR, UNLOAD_FILE_ERROR

} elf_err_e;

elf_err_e elf_load_file(elf_ctx_t *elf_ctx, const char *elf_path_name);

elf_err_e elf_unload(elf_ctx_t *elf_ctx);

#endif

