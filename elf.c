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

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#if defined(__unix__)
#define _GNU_SOURCE

#include <errno.h>

#endif

/*
    If there's a already installed version of the library, 
    continue to search in the include directory of this project.
*/

#include "libelf/elf.h"

#include "elf-error.h"
#include "elf-string-data.h"

static elf_err_e        INTERNAL_elf_load_file(const char *elf_path_name, elf_ctx_t *elf_ctx);
static elf_err_e        INTERNAL_elf_unload(elf_ctx_t *elf_ctx);

static elf_err_e        INTERNAL_elf_parser(elf_ctx_t *elf_ctx);

static bool             INTERNAL_elf_is_elf(const elf_ctx_t *elf_ctx);

static int32_t          INTERNAL_elf_get_type(const char *type_name, elf_ctx_t *elf_ctx);
static const char*      INTERNAL_elf_type_to_str(const char *type_name, int32_t type_value);

static elf_arch_e       INTERNAL_elf_get_arch(const elf_ctx_t *elf_ctx);
static elf_cpu_encode_e INTERNAL_elf_get_encode(const elf_ctx_t *elf_ctx);
static elf_version_e    INTERNAL_elf_get_version(const elf_ctx_t *elf_ctx);

static const char*      INTERNAL_elf_arch_to_str(const elf_arch_e arch_type);
static const char*      INTERNAL_elf_encode_to_str(const elf_cpu_encode_e encode_type);
static const char*      INTERNAL_elf_version_to_str(const elf_version_e version_type);

static const char*      INTERNAL_elf_error_to_str(const elf_err_e error);
static elf_err_e        INTERNAL_elf_get_error(const elf_ctx_t *elf_ctx);
static size_t           INTERNAL_elf_error_format(char *buffer, const size_t buffer_size, const elf_ctx_t *elf_ctx);

/* Protecting the library method by exporting a wrapper method */

elf_err_e elf_load_file(const char *elf_path_name, elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_load_file(elf_path_name, elf_ctx);
}

static elf_err_e INTERNAL_elf_load_file(const char *elf_path_name, elf_ctx_t *elf_ctx)
{

    memset(elf_ctx, 0, sizeof(elf_ctx_t));

    if (elf_path_name == NULL)
        return elf_ctx->error_id = ELF_E_LOAD_FILE;
    
    elf_ctx->elf_name = strdup(elf_path_name);

    const FILE *local_file_ptr = elf_ctx->elf_file_ptr = fopen(elf_path_name, "rb");

    if (local_file_ptr == NULL)
    {
#if defined(__unix__)
        elf_ctx->error_record = strerror(errno);
#endif
        return elf_ctx->error_id = ELF_E_LOAD_FILE;
    }

    return elf_ctx->error_id = ELF_E_OK;
}

elf_err_e elf_unload(elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_unload(elf_ctx);
}

elf_err_e elf_parser(elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_parser(elf_ctx);
}

bool elf_is_elf(const elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_is_elf(elf_ctx);
}

elf_arch_e elf_get_arch(const elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_get_arch(elf_ctx);
}

elf_cpu_encode_e elf_get_encode(const elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_get_encode(elf_ctx);
}

elf_version_e elf_get_version(const elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_get_version(elf_ctx);
}

int32_t elf_get_type(const char *type_name, elf_ctx_t *elf_ctx)
{
    assert(type_name != NULL);
    assert(elf_ctx != NULL);

    return INTERNAL_elf_get_type(type_name, elf_ctx);
}

const char* elf_type_to_str(const char *type_name, const int32_t type_value)
{
    assert(type_name != NULL);

    return INTERNAL_elf_type_to_str(type_name, type_value);
}

const char* elf_error_to_str(elf_err_e error)
{
    return INTERNAL_elf_error_to_str(error);
}

elf_err_e elf_get_error(const elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_get_error(elf_ctx);
}

const char* elf_arch_to_str(const elf_arch_e arch_type)
{
    return INTERNAL_elf_arch_to_str(arch_type);
}

const char* elf_encode_to_str(const elf_cpu_encode_e encode_type)
{
    return INTERNAL_elf_encode_to_str(encode_type);
}

const char* elf_version_to_str(const elf_version_e version_type)
{
    return INTERNAL_elf_version_to_str(version_type);
}

size_t elf_error_format(char *buffer, size_t buffer_size, const elf_ctx_t *elf_ctx)
{
    assert(buffer && buffer_size);
    assert(elf_ctx != NULL);

    return INTERNAL_elf_error_format(buffer, buffer_size, elf_ctx);
}

static elf_err_e INTERNAL_elf_unload(elf_ctx_t *elf_ctx)
{
    FILE *local_file_ptr = elf_ctx->elf_file_ptr;

    if (local_file_ptr == NULL)
        return elf_ctx->error_id = ELF_E_UNLOAD_FILE;
    
    free((char*)elf_ctx->elf_name);
    
    fclose(local_file_ptr);

    memset(elf_ctx, 0, sizeof(elf_ctx_t));

    return elf_ctx->error_id = ELF_E_OK;

}

static elf_err_e INTERNAL_elf_parser(elf_ctx_t *elf_ctx)
{
    FILE *elfptr = elf_ctx->elf_file_ptr;
    
    if (elfptr == NULL)
        return elf_ctx->error_id = ELF_E_LOAD_FILE;
    
    const int header_br = fread((void*)&elf_ctx->elf, 
        1, sizeof(elf_header64_t), elfptr);

    if (header_br != sizeof(elf_header64_t))
        return elf_ctx->error_id = ELF_E_CANT_READ;

    if (INTERNAL_elf_is_elf(elf_ctx) == false)
        return elf_ctx->error_id = ELF_E_INVALID_ELF;

    const elf_arch_e arch = elf_ctx->arch_type = elf_ctx->elf.elf_ident[4];
    if (arch == EA_INVALID)
        return elf_ctx->error_id = ELF_E_INVALID_ARCH;

    const elf_cpu_encode_e encode = elf_ctx->cpu_encode = elf_ctx->elf.elf_ident[5];
    if (encode == ECE_UNKNOW)
        return elf_ctx->error_id = ELF_E_UNKNOW_CPU_ENCODE;

    const elf_version_e ver = elf_ctx->version = elf_ctx->elf.elf_ident[6];

    if (ver == EV_INVALID)
        return elf_ctx->error_id = ELF_E_INVALID_VERSION;

    return elf_ctx->error_id = ELF_E_OK;

}

static bool INTERNAL_elf_is_elf(const elf_ctx_t *elf_ctx)
{
    static const uint8_t valid_elf_magic[] = {0x7f, 'E', 'L', 'F'};

    return memcmp(&elf_ctx->elf, valid_elf_magic, 
        sizeof(valid_elf_magic)) == 0;
}

static elf_arch_e INTERNAL_elf_get_arch(const elf_ctx_t *elf_ctx)
{
    return elf_ctx->arch_type;
}

static elf_cpu_encode_e INTERNAL_elf_get_encode(const elf_ctx_t *elf_ctx)
{
    return elf_ctx->cpu_encode;
}

static elf_version_e INTERNAL_elf_get_version(const elf_ctx_t *elf_ctx)
{
    return elf_ctx->version;
}

static int32_t INTERNAL_elf_get_type(const char *type_name, elf_ctx_t *elf_ctx)
{
    const char *const *type_ptr = type_name_info;

    int32_t select = -1;

    for (int code = 0; type_ptr[code]; code++)
        if (strcmp(type_ptr[code], type_name) == 0)
            select = code;
    
    switch (select)
    {
    case 0: select = elf_ctx->arch_type;    break;
    case 1: select = elf_ctx->cpu_encode;   break;
    case 2: select = elf_ctx->version;      break;
    default:                                break;
    }

    /* Invalid state */
    if (select == -1)
        elf_ctx->error_id = ELF_E_INVALID_TYPE_NAME;

    return select;
}

static const char* INTERNAL_elf_type_to_str(const char *type_name, const int32_t type_value)
{
    const char *const *type_ptr = type_name_info;

    int select = -1;

    for (int code = 0; type_ptr[code]; code++)
        if (strcmp(type_ptr[code], type_name) == 0)
            select = code;

    const char * const *str_list = NULL;
    int32_t str_max_index = 0;
        
    switch(select)
    {
    case 0: 
        str_list = elf_arch_str_list;
        str_max_index = EA_NULL_VALUE;
        break;
    case 1:
        str_list = elf_encode_str_list;
        str_max_index = ECE_NULL_VALUE;
        break;
    case 2: str_list = elf_version_str_list;
        str_max_index = EV_NULL_VALUE;
        break;
    default: break;
    }

    if (str_list == NULL)
        return elf_error_str_list[ELF_E_NULL_VALUE];

    if (type_value >= str_max_index)
        return elf_error_str_list[ELF_E_NULL_VALUE];

    return str_list[type_value];
}

static const char* INTERNAL_elf_error_to_str(const elf_err_e error)
{
    if (error >= ELF_E_NULL_VALUE)
        /* The error message is longer than the error list */
        return elf_error_str_list[ELF_E_NULL_VALUE];

    const char *error_message = elf_error_str_list[error];

    assert(error_message);

    return error_message;
}

static const char* INTERNAL_elf_arch_to_str(const elf_arch_e arch_type)
{
    if (arch_type >= EA_NULL_VALUE)
        /* The error message is longer than the error list */
        return elf_error_str_list[ELF_E_NULL_VALUE];

    const char *arch_str = elf_arch_str_list[arch_type];
    return arch_str;
}

static const char* INTERNAL_elf_encode_to_str(const elf_cpu_encode_e encode_type)
{
    if (encode_type >= ECE_NULL_VALUE)
        /* The error message is longer than the error list */
        return elf_error_str_list[ELF_E_NULL_VALUE];

    const char *encode_str = elf_encode_str_list[encode_type];
    return encode_str;
}

static const char* INTERNAL_elf_version_to_str(const elf_version_e version_type)
{
    if (version_type >= EV_NULL_VALUE)
        /* The error message is longer than the error list */
        return elf_error_str_list[ELF_E_NULL_VALUE];

    const char *version_str = elf_version_str_list[version_type];
    return version_str;
}

static elf_err_e INTERNAL_elf_get_error(const elf_ctx_t *elf_ctx)
{
    return elf_ctx->error_id;
}

size_t INTERNAL_elf_error_format(char *buffer, const size_t buffer_size, 
    const elf_ctx_t *elf_ctx)
{
    size_t format_return = snprintf(buffer, buffer_size, "error: %s, because: %s",
        INTERNAL_elf_error_to_str(INTERNAL_elf_get_error(elf_ctx)), elf_ctx->error_record);
    
    return format_return;
}
