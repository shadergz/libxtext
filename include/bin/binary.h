#ifndef BIN_BINARY_H
#define BIN_BINARY_H

#include "bintypes.h"
#include "elftypes.h"

/* Load and unload functions declaration */
__attribute__((unused)) extern bool				bin_load_file			(const char *pathname, BinCtx_t *bin);
__attribute__((unused)) extern bool       		bin_unload_file			(BinCtx_t *bin);
__attribute__((unused)) extern bool 			bin_finish				(BinCtx_t *bin);
/* Binary manipulation functions */
__attribute__((unused)) extern bool 			bin_parser				(BinCtx_t *bin);
/* Getting functions */
/* The returned pointer by bin_get_filename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
*/
__attribute__((unused)) extern const char*		bin_obj_get_filename	(const BinCtx_t *bin);
__attribute__((unused)) extern size_t       	bin_obj_get_size		(const BinCtx_t *bin);
__attribute__((unused)) extern BinType_t    	bin_obj_get_type		(const BinCtx_t *bin);
__attribute__((unused)) extern const char*  	bin_obj_type_to_str		(BinType_t bin_type);
__attribute__((unused)) extern bool 			bin_obj_get_symhdr		(ObjectSymbolCtx_t *symbolCtx,
																		 BinCtx_t *bin);
/* Error handler functions */
__attribute__((unused)) extern const BinError_t	bin_error_get_last		(const BinCtx_t *bin);
__attribute__((unused)) extern const BinError_t bin_error_get			(const BinCtx_t *bin);
__attribute__((unused)) extern const char*		bin_error_to_str		(BinError_t error_value);

#endif

