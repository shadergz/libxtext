#include "identify.h"
#include "elfdata.h"

#include <string.h>

bin_type_e identify_unknow_magic(const unsigned char magic_header[4])
{
    if (memcmp(magic_header, ELF_signature, sizeof(ELF_signature)) == 0)
        /* The binary is a ELF file */
        return BT_ELF_FILE;

    return BT_UNKNOW;
}

