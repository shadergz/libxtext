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

static elf_err_e    INTERNAL_elf_load_file(elf_ctx_t *elf_ctx, const char *elf_path_name);

static elf_err_e    INTERNAL_elf_unload(elf_ctx_t *elf_ctx);

static elf_err_e    INTERNAL_elf_parser(elf_ctx_t *elf_ctx);

static bool         INTERNAL_elf_is_elf(elf_ctx_t *elf_ctx);

static const char*  INTERNAL_elf_error_to_str(elf_err_e error);
static elf_err_e    INTERNAL_elf_get_error(elf_ctx_t *elf_ctx);
static char*        INTERNAL_error_format(char *buffer, size_t buffer_size, elf_ctx_t *elf_ctx);

/* Protecting the library method by exporting a wrapper method */

elf_err_e elf_load_file(elf_ctx_t *elf_ctx, const char *elf_path_name)
{
    assert(elf_ctx != NULL);

    return INTERNAL_elf_load_file(elf_ctx, elf_path_name);
}

static elf_err_e INTERNAL_elf_load_file(elf_ctx_t *elf_ctx, const char *elf_path_name)
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

    /* Enable a way for the user to collect informations from the ELF file */
    elf_ctx->FUNC_elf_unload        =   INTERNAL_elf_unload;
    elf_ctx->FUNC_elf_parser        =   INTERNAL_elf_parser;
    elf_ctx->FUNC_elf_get_error     =   INTERNAL_elf_get_error;
    elf_ctx->FUNC_error_format      =   INTERNAL_error_format;

    return elf_ctx->error_id = ELF_E_OK;

}

elf_err_e elf_unload(elf_ctx_t *elf_ctx)
{
    assert(elf_ctx);

    if (elf_ctx->FUNC_elf_unload == NULL)
        return elf_ctx->error_id = ELF_E_INVALID_FUNC;

    return elf_ctx->FUNC_elf_unload(elf_ctx);
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

elf_err_e elf_parser(elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    if (elf_ctx->FUNC_elf_parser == NULL)
        return elf_ctx->error_id = ELF_E_INVALID_FUNC;

    return elf_ctx->FUNC_elf_parser(elf_ctx);
}

static elf_err_e INTERNAL_elf_parser(elf_ctx_t *elf_ctx)
{
    FILE *elfptr = elf_ctx->elf_file_ptr;
    
    if (elfptr == NULL)
        return elf_ctx->error_id = ELF_E_LOAD_FILE;
    
    const int header_br = fread((void*)&elf_ctx->elf_buffer_header, 
        1, sizeof(elf_header64_t), elfptr);

    if (header_br == 1)
        return elf_ctx->error_id = ELF_E_CANT_READ;

    /* Now the user can check if the ELF is really a ELF or not */
    elf_ctx->FUNC_elf_is_elf = INTERNAL_elf_is_elf;

    if (elf_ctx->FUNC_elf_is_elf(elf_ctx) == false)
        return elf_ctx->error_id = ELF_E_INVALID_ELF;

    return elf_ctx->error_id = ELF_E_OK;

}

bool elf_is_elf(elf_ctx_t *elf_ctx)
{
    assert(elf_ctx != NULL);

    if (elf_ctx->FUNC_elf_is_elf == NULL)
    {
        elf_ctx->error_id = ELF_E_INVALID_FUNC;
        return false;
    }

    return elf_ctx->FUNC_elf_is_elf(elf_ctx);
}

static bool INTERNAL_elf_is_elf(elf_ctx_t *elf_ctx)
{
    static const uint8_t valid_elf_magic[] = {0x7f, 'E', 'L', 'F'};

    return memcmp(&elf_ctx->elf_buffer_header, valid_elf_magic, 
        sizeof(valid_elf_magic)) == 0;
}

const char* elf_error_to_str(elf_err_e error)
{   
    return INTERNAL_elf_error_to_str(error);
}

static const char* INTERNAL_elf_error_to_str(elf_err_e error)
{
    if (error >= NULL_VALUE)
        /* The error message is longer than the error list */
        return elf_error_str_list[NULL_VALUE];

    const char *error_message = elf_error_str_list[error];

    assert(error_message);

    return error_message;
}

elf_err_e elf_get_error(elf_ctx_t *elf_ctx)
{
    assert(elf_ctx);

    if (elf_ctx->FUNC_elf_get_error == NULL)
        return elf_ctx->error_id = ELF_E_INVALID_FUNC;

    return elf_ctx->FUNC_elf_get_error(elf_ctx);
}

static elf_err_e INTERNAL_elf_get_error(elf_ctx_t *elf_ctx)
{
    return elf_ctx->error_id;
}

char* elf_error_format(char *buffer, size_t buffer_size, elf_ctx_t *elf_ctx)
{
    assert(buffer && buffer_size);
    assert(elf_ctx);

    if (elf_ctx->FUNC_error_format == NULL)
    {
        snprintf(buffer, buffer_size, "%s", elf_error_to_str(NULL_VALUE));
        elf_ctx->error_id = ELF_E_INVALID_FUNC;
        return buffer;
    }

    return elf_ctx->FUNC_error_format(buffer, buffer_size, elf_ctx);
}

static char* INTERNAL_error_format(char *buffer, size_t buffer_size, elf_ctx_t *elf_ctx)
{
    snprintf(buffer, buffer_size, "error: %s, because: %s",
        elf_error_to_str(elf_get_error(elf_ctx)), elf_ctx->error_record);
    
    return buffer;

}
