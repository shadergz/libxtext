#ifndef XTEXT_BIN_H
#define XTEXT_BIN_H

#include <xtext/common.h>
#include <xtext/elf_content.h>
#include <xtext/ext_types.h>

typedef bool (*cb_sym_hdr_t) (xtext_ctx_t *ctx,
                              const elfsec_hdr_t *func_object,
                              elfsec_hdr_t *user_object);
typedef bool (*cb_symb_t) (xtext_ctx_t *ctx, const object_symb_t *func_object,
                           object_symb_t *user_object);

/* Load and unload functions declaration */
XTEXT_UNUSED XTEXT_EXTERNAL bool bin_load_file (const char *pathname,
                                                xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL bool xtext_unload_file (xtext_ctx_t *xtext);
XTEXT_UNUSED XTEXT_EXTERNAL bool bin_finish (xtext_ctx_t *xtext);
/* Binary manipulation functions */
XTEXT_UNUSED XTEXT_EXTERNAL bool xtext_parser (xtext_ctx_t *xtext_ctx);
/* Getting functions */
/* The returned pointer by bin_get_filename is allocated using malloc,
 * after call bin_finish function, this pointer will be destroyed.
 */
XTEXT_UNUSED XTEXT_EXTERNAL bool etext_obj_exist (const char *pathname);
XTEXT_UNUSED XTEXT_EXTERNAL bool xtext_obj_can_read (const char *pathname);
XTEXT_UNUSED XTEXT_EXTERNAL const char *
xtext_obj_get_filename (xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL size_t xtext_obj_get_size (xtext_ctx_t *xtext);
XTEXT_UNUSED XTEXT_EXTERNAL size_t
xtext_obj_memory_size (xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL xtext_type_t
xtext_obj_get_type (xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL const char *
xtext_obj_type_to_str (xtext_type_t bin_type);
XTEXT_UNUSED XTEXT_EXTERNAL bool
xtext_obj_is_loaded (const xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL bool xtext_obj_is_ELF (xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL cpu_endian_e
xtext_obj_get_endian (xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL const char *
xtext_cpu_endian_to_str (cpu_endian_e cpu_endian);
XTEXT_UNUSED XTEXT_EXTERNAL const char *
xtext_class_to_str (class_bits_e class_bits);
XTEXT_UNUSED XTEXT_EXTERNAL bool
xtext_obj_class_is_32b (xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL bool
etext_obj_class_is_64b (xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL bool
xtext_obj_symhdr_foreach (xtext_ctx_t *xtext_ctx, cb_sym_hdr_t user_func,
                          elfsec_hdr_t *use_object);
XTEXT_UNUSED XTEXT_EXTERNAL bool
xtext_obj_symbols_foreach (xtext_ctx_t *xtext_ctx, cb_symb_t user_func,
                           object_symb_t *user_object);

/* Error handler functions */
XTEXT_UNUSED XTEXT_EXTERNAL const xtext_error_e
xtext_error_get_last (const xtext_ctx_t *xtext_ctx);
XTEXT_UNUSED XTEXT_EXTERNAL const xtext_error_e
xtext_error_get (const xtext_ctx_t *xtext);
XTEXT_UNUSED XTEXT_EXTERNAL const char *
xtext_error_to_str (xtext_error_e error_value);
XTEXT_UNUSED class_bits_e
xtext_obj_get_class (xtext_ctx_t *xtext_ctx);

#endif
