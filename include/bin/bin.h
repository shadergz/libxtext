#ifndef LIBBIN_BIN_H
#define LIBBIN_BIN_H

#include "bintypes.h"

/* Load and unload functions */

__attribute__((unused)) BinError_t          BinLoadFile(const char *pathname, BinCtx_t *bin);

__attribute__((unused)) BinError_t          BinUnloadFile(BinCtx_t *bin);

__attribute__((unused)) BinError_t          BinFinish(BinCtx_t *bin);

/* Binary manipulation functions */
__attribute__((unused)) BinError_t          BinParser(BinCtx_t *bin);

/* Getting functions */

/* The returned pointer by BinGetFilename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
*/
__attribute__((unused)) const char*         BinGetFilename(const BinCtx_t *bin);

__attribute__((unused)) const BinError_t    BinGetLastError(const BinCtx_t *bin);

__attribute__((unused)) size_t              BinGetBinarySize(const BinCtx_t *bin);

__attribute__((unused)) BinType_t           BinGetType(const BinCtx_t *bin);

__attribute__((unused)) const char*         BinBinaryTypeToStr(const BinType_t bin_type);

/* Error handler functions */
__attribute__((unused)) const char*         BinErrorToStr(const BinError_t error_value);

#endif

