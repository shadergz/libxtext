#ifndef LIBBIN_BIN_H
#define LIBBIN_BIN_H

#include "bintypes.h"

/* Load and unload functions */

BINERR_t            BinLoadFile(const char *pathname, BinCtx_t *bin);
BINERR_t            BinUnloadFile(BinCtx_t *bin);
BINERR_t            BinFinish(BinCtx_t *bin);

/* Binary manipulation functions */
BINERR_t            BinParser(BinCtx_t *bin);

/* Getting functions */

/* The returned pointer by BinGetFilename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
*/
const char*         BinGetFilename(const BinCtx_t *bin);
const BINERR_t      BinGetLastError(const BinCtx_t *bin);
size_t              BinGetBinarySize(const BinCtx_t *bin);

BinType_t           BinGetType(const BinCtx_t *bin);
const char*         BinaryTypeToStr(const BinType_t bin_type);

/* Error handler functions */
const char*         BinErrorToStr(const BINERR_t error_value);

#endif

