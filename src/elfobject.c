#include <assert.h>

#include "bin/binary.h"
#include "object.h"

/* When reading a file format object, some parts are important for know what the file really is,
 * in the case of binaries files like ELF format, there is a magic header into the ident segment that has
 * this information describe bellow for identifier and classifier the object by himself
*/

__attribute__((visibility("hidden"))) const uint8_t ELF_Signature[4] =
{
	ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3
};

__attribute__((visibility("hidden"))) const ELFHeader_Ctx_t* ELF_Get_Header(BinCtx_t *bin)
{
	assert(bin);
	/* Checking if object is a ELF format */
	if (bin->binaryType != BT_ELF_FILE)
	{
		bin->errorStatus = BIN_E_NOT_A_ELF;
		return false;
	}

	/* Getting the header address (it's at the file begging, but I will keep this) */
	const BinMap_t *map = &bin->binaryMap;
	const ELFHeader_Ctx_t *headerCtx = (const ELFHeader_Ctx_t *)BIN_ADD_PTR(0, map->mapStart);

	return headerCtx;
}

