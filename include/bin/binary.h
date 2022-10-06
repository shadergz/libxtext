#ifndef BIN_BINARY_H
#define BIN_BINARY_H

#include "bintypes.h"
#include "elftypes.h"
#include "common.h"

typedef bool (*CallbackSymHdrFunc_t) (BinCtx_t *ctx, const ELFSecHdr_t    *funcObject, ELFSecHdr_t    *userObject);
typedef bool (*CallbackSymbolsFunc_t)(BinCtx_t *ctx, const ObjectSymbol_t *funcObject, ObjectSymbol_t *userObject);

/* Load and unload functions declaration */
UNUSED EXTERNAL bool                bin_load_file			(const char *pathname, BinCtx_t *binCtx);
UNUSED EXTERNAL bool                bin_unload_file			(BinCtx_t *bin);
UNUSED EXTERNAL bool                bin_finish				(BinCtx_t *bin);
/* Binary manipulation functions */
UNUSED EXTERNAL bool                bin_parser				(BinCtx_t *binCtx);
/* Getting functions */
/* The returned pointer by bin_get_filename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
*/
UNUSED EXTERNAL bool                bin_obj_exist           (const char *pathname);
UNUSED EXTERNAL bool                bin_obj_can_read        (const char *pathname);
UNUSED EXTERNAL const char*         bin_obj_get_filename    (BinCtx_t *binCtx);
UNUSED EXTERNAL size_t              bin_obj_get_size        (BinCtx_t *bin);
UNUSED EXTERNAL size_t              bin_obj_memory_size     (BinCtx_t *binCtx);
UNUSED EXTERNAL BinType_t           bin_obj_get_type        (BinCtx_t *binCtx);
UNUSED EXTERNAL const char*         bin_obj_type_to_str     (BinType_t binType);
UNUSED EXTERNAL bool                bin_obj_is_loaded       (const BinCtx_t *binCtx);
UNUSED EXTERNAL bool                bin_obj_is_ELF          (BinCtx_t *binCtx);
UNUSED EXTERNAL CPU_Endian_e        bin_obj_get_endian      (BinCtx_t *binCtx);
UNUSED EXTERNAL const char*         bin_cpu_endian_to_str   (CPU_Endian_e cpuEndian);
UNUSED EXTERNAL const char*         bin_class_to_str        (ClassBits_e classBits);
UNUSED EXTERNAL ClassBits_e         bin_obj_get_class       (BinCtx_t *binCtx);
UNUSED EXTERNAL bool                bin_obj_class_is_32b    (BinCtx_t *binCtx);
UNUSED EXTERNAL bool                bin_obj_class_is_64b    (BinCtx_t *binCtx);
UNUSED EXTERNAL bool                bin_obj_symhdr_foreach  (BinCtx_t *binCtx, CallbackSymHdrFunc_t userFunc,
                                                             ELFSecHdr_t *useObject);
UNUSED EXTERNAL bool                bin_obj_symbols_foreach (BinCtx_t *binCtx,
                                                             CallbackSymbolsFunc_t userFunc,
                                                             ObjectSymbol_t *userObject);
/* Error handler functions */
UNUSED EXTERNAL const BinError_e    bin_error_get_last      (const BinCtx_t *binCtx);
UNUSED EXTERNAL const BinError_e    bin_error_get           (const BinCtx_t *bin);
UNUSED EXTERNAL const char*         bin_error_to_str        (BinError_e errorValue);

#endif

