#ifndef BIN_OBJECT_H
#define BIN_OBJECT_H

#define BIN_ADD_PTR(relative, map)\
	(relative + map)

const ELFHeader_Ctx_t* ELF_Get_Header(BinCtx_t *bin);

#endif
