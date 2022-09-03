#ifndef BIN_BINARY_H
#define BIN_BINARY_H

#include "bintypes.h"
#include "elftypes.h"

/* Load and unload functions declaration */
__attribute__((unused)) extern bool				BinLoadFile			(const char *pathname, BinCtx_t *bin);
__attribute__((unused)) extern bool       		BinUnloadFile		(BinCtx_t *bin);
__attribute__((unused)) extern bool 			BinFinish			(BinCtx_t *bin);
/* Binary manipulation functions */
__attribute__((unused)) extern bool 			BinParser			(BinCtx_t *bin);
/* Getting functions */
/* The returned pointer by BinGetFilename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
*/
__attribute__((unused)) extern const char*		BinGetFilename		(const BinCtx_t *bin);
__attribute__((unused)) extern const BinError_t	BinGetLastError		(const BinCtx_t *bin);
__attribute__((unused)) extern size_t       	BinGetObjectSize	(const BinCtx_t *bin);
__attribute__((unused)) extern BinType_t    	BinObjectGetType	(const BinCtx_t *bin);
__attribute__((unused)) extern const char*  	BinObjectTypeToStr	(BinType_t bin_type);
__attribute__((unused)) extern bool 			BinGetObjectSymHdr	(ObjectSymbolCtx_t *symbolCtx,
																	 BinCtx_t *bin);
/* Error handler functions */
__attribute__((unused)) extern const BinError_t BinGetError			(const BinCtx_t *bin);
__attribute__((unused)) extern const char*		BinErrorToStr		(BinError_t error_value);

#endif

