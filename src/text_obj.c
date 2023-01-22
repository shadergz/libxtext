#include <xtext/ext_bin.h>

/* When reading a file with object format, is really important to known what
 * the file exactly is, binaries files like the ELF format has a magic header
 * value field into the ```ident segment```, that provides information that
 * should be comparable with the elf_signature data bellow, otherwise in
 * failure, reject the file with this signature!
 */

XTEXT_INTERNAL const uint8_t elf_signature[4]
    = { ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3 };
