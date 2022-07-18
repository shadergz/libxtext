#ifndef LIBBIN_BIN_H
#define LIBBIN_BIN_H

#include "bintypes.h"

/* Load and unload functions */

BinError_t            BinLoadFile(const char *pathname, BinCtx_t *bin);
BinError_t            BinUnloadFile(BinCtx_t *bin);
BinError_t            BinFinish(BinCtx_t *bin);

/* Binary manipulation functions */
BinError_t            BinParser(BinCtx_t *bin);

/* Getting functions */

/* The returned pointer by BinGetFilename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
*/
const char*         BinGetFilename(const BinCtx_t *bin);
const BinError_t      BinGetLastError(const BinCtx_t *bin);
size_t              BinGetBinarySize(const BinCtx_t *bin);

BinType_t           BinGetType(const BinCtx_t *bin);
const char*         BinaryTypeToStr(const BinType_t bin_type);

/* Error handler functions */
const char*         BinErrorToStr(const BinError_t error_value);

#endif

