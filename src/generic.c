#include <assert.h>
#include <string.h>
#include <malloc.h>

#if defined(__unix__)

#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <unistd.h>

#endif

#include "bin/bin.h"

#include "binman.h"
#include "binparser.h"
#include "error.h"
#include "memhand.h"
#include "unload.h"

static const char*      bin_get_filename_ext(const bin_ctx_t *bin);
static const bin_err_e  bin_get_last_error_ext(const bin_ctx_t *bin);
static size_t           bin_get_binary_size_ext(const bin_ctx_t *bin);
static bin_type_e       bin_get_binary_type_ext(const bin_ctx_t *bin);
static const char*      bin_binary_type_to_str_ext(const bin_type_e bin_type);

bin_err_e bin_load_file(const char *pathname, bin_ctx_t *bin)
{
    assert(pathname != NULL);
    assert(bin != NULL);

    return bin_load_file_ext(pathname, bin);
}

bin_err_e bin_finish(bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_finish_ext(bin);
}

bin_err_e bin_unload_file(bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_finish(bin);
}

bin_err_e bin_parser(bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_parser_ext(bin);
}

const char* bin_error_to_str(const bin_err_e error_value)
{

    return bin_error_to_str_ext(error_value);
}

const char* bin_get_filename(const bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_get_filename_ext(bin);
}

const bin_err_e bin_get_last_error(const bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_get_last_error_ext(bin);
}

size_t bin_get_binary_size(const bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_get_binary_size_ext(bin);
}

bin_type_e bin_get_binary_type(const bin_ctx_t *bin)
{
    assert(bin != NULL);

    return bin_get_binary_type_ext(bin);
}

const char* bin_binary_type_to_str(const bin_type_e bin_type)
{
    return bin_binary_type_to_str_ext(bin_type);
}

static const char* bin_get_filename_ext(const bin_ctx_t *bin)
{
    return bin->pathname;
}
static const bin_err_e bin_get_last_error_ext(const bin_ctx_t *bin)
{
    return bin->error_status;
}
static size_t bin_get_binary_size_ext(const bin_ctx_t *bin)
{
    return bin->binary_file_size;
}

static bin_type_e bin_get_binary_type_ext(const bin_ctx_t *bin)
{
    return bin->binary_type;
}

const char * const binary_type_list[];

static const char* bin_binary_type_to_str_ext(const bin_type_e bin_type)
{
    if (bin_type >= BT_FINAL_NULL_VALUE)
        return binary_type_list[BT_FINAL_NULL_VALUE];
    
    return binary_type_list[bin_type];
}

const char * const binary_type_list[] = {

    // BT_UNKNOW
    "unknow (not recognized, a.k.a UNK)",

    // BT_PE_FILE
    "portable executable (PE)",

    // BT_ELF_FILE
    "executable and linkable format (ELF)",

    // BT_FINAL_NULL_VALUE
    "(null)",
    
    NULL
};

