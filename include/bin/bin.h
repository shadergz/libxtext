#ifndef LIBBIN_BIN_H
#define LIBBIN_BIN_H

#include "bintypes.h"

/* Load a binary file from his pathname */
bin_err_e bin_load_file(const char *pathname, bin_ctx_t *bin);

/* Unload the current file */
bin_err_e bin_unload_file(bin_ctx_t *bin);

/* Cleaning the context structure */
bin_err_e bin_finish(bin_ctx_t *bin);

#endif

