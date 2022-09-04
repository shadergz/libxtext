#include "bin/binary.h"

/* When reading a file format object, some parts are important for know what the file really is,
 * in the case of binaries files like ELF format, there is a magic header into the ident segment that has
 * this information describe bellow for identifier and classifier the object by himself
*/

INTERNAL const uint8_t ELFSignature[4] = {
	ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3
};
