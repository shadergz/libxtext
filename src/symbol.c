#include "bin/bintypes.h"
#include "bin/elftypes.h"

#include "object.h"

__attribute__((unused)) bool bin_obj_get_symhdr(ObjectSymbolCtx_t *symbolCtx, BinCtx_t *bin)
{
	const ELFHeader_Ctx_t *elfHeaderCtx = ELF_Get_Header(bin);
	(void)elfHeaderCtx;

	return true;
}
