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

#include "elf-types.h"

elf_err_e elf_load_file(elf_ctx_t *elf_ctx, const char *elf_path_name);

elf_err_e elf_unload(elf_ctx_t *elf_ctx);

/* ELF */
elf_err_e elf_parser(elf_ctx_t *elf_ctx);

/* Checking ELF API functions */
bool elf_is_elf(elf_ctx_t *elf_ctx);

/* Error API handler functions */

const char* elf_error_to_str(const elf_err_e error);

elf_err_e elf_get_error(elf_ctx_t *elf_ctx);

char* elf_error_format(char *buffer, size_t buffer_size, elf_ctx_t *elf_ctx);

#endif

