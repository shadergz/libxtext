#include "error.h"

const char* bin_error_to_str_ext(const bin_err_e error_value)
{
    if (error_value >= BIN_E_FINAL_NULL_VALUE)
        return bin_e_str_list[BIN_E_FINAL_NULL_VALUE];
    
    return bin_e_str_list[error_value];
    
}

const char * const bin_e_str_list[] = {

    // BIN_E_OK
    "everything is ok",

    // BIN_E_OPEN_FILE
    "can't open the file, maybe the file not exist",

    // BIN_E_CLOSE_FILE
    "can't close the file, maybe has been deleted",

    // BIN_E_MALLOCATION_ERROR
    "a recently allocation using the malloc function has been failed",

    // BIN_E_IS_EMPTY
    "the supposed binary file is empty",

    // BIN_E_ALREDY_PARSED
    "the executable has been alredy parsed",

#if defined(__unix__)

    // BIN_E_FSTAT_FAILED
    "a recently call for function fstat has returned a fail value",

    // BIN_E_MMAP_FAILED
    "a recently call for mmap has failed",

    // BIN_E_MUNMAP_FAILED
    "a recently call for munmap has failed",

#endif

    // BIN_E_CANT_READ
    "A recently try to read a chunk of memory has been failed",

    // BIN_E_INVALID_FILE
    "a invalid file name has been passed",

    // BIN_E_FINAL_NULL_VALUE
    /* Invalid error values */
    "(null)",

    NULL
};
