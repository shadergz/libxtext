#ifndef LIBBIN_ERROR_H
#define LIBBIN_ERROR_H

#include "bin/bin.h"

__attribute__((visibility("hidden"))) extern const char * const bin_e_str_list[];
__attribute__((visibility("hidden"))) extern const char* bin_error_to_str_ext(const bin_err_e error_value);

#endif
