#ifndef LIBBIN_BIN_H
#define LIBBIN_BIN_H

#include "bintypes.h"

/* -------- Load and unload operation functions -------- */

bin_err_e bin_load_file(const char *pathname, bin_ctx_t *bin);
bin_err_e bin_unload_file(bin_ctx_t *bin);

/* -------- Binary manipulation functions -------- */
bin_err_e bin_parser(bin_ctx_t *bin);

/* -------- Getting functions -------- */

/* The returned pointer by bin_get_filename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
*/
const char* bin_get_filename(const bin_ctx_t *bin);
const bin_err_e bin_get_last_error(const bin_ctx_t *bin);
size_t bin_get_binary_size(const bin_ctx_t *bin);

/* -------- Error handler functions -------- */
const char* bin_error_to_str(const bin_err_e error_value);

/* -------- Unload operation functions -------- */
bin_err_e bin_finish(bin_ctx_t *bin);

#endif

