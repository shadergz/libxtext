#include <assert.h>

#include "bin/binary.h"

__attribute__((unused)) bool BinGetObjectSymHdr(ObjectSymbolCtx_t *symbolCtx, BinCtx_t *bin)
{
	assert(bin);
	/* Checking if object is a ELF format */
	if (bin->binaryType != BT_ELF_FILE)
	{
		bin->errorStatus = BIN_E_NOT_A_ELF;
		return false;
	}
	(void)symbolCtx;

	return true;
}