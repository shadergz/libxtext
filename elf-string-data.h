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

#ifndef LIBELF_ELFSL_H
#define LIBELF_ELFSL_H

extern __attribute__((visibility("hidden"))) const char *type_name_info[];

extern __attribute__((visibility("hidden"))) const char * const elf_arch_str_list[];

extern __attribute__((visibility("hidden"))) const char * const elf_encode_str_list[];

extern __attribute__((visibility("hidden"))) const char * const elf_version_str_list[];

#endif


