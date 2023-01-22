
#include <stdlib.h>

#if defined(__unix__)
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <assert.h>
#include <string.h>

#include <xtext/ext_bin.h>
#include <xtext/elf_content.h>

#include <local/local_obj.h>

XTEXT_UNUSED const char *LIBRARY_NAME = "libxtext";
XTEXT_UNUSED const char *LIBRARY_VERSION = "0.0.4";

#define XTEXT_HAND_IS_READABLE(ctx, retValue)                                   \
  if (xtext_obj_is_loaded (ctx) == false)                                     \
    {                                                                         \
      (ctx)->error_instatus = XTEXT_E_CANT_READ;                              \
      return retValue;                                                        \
    }                                                                         \
  (void)ctx

XTEXT_UNUSED bool
etext_obj_exist (const char *pathname)
{
  return access (pathname, F_OK) == 0;
}

XTEXT_UNUSED bool
xtext_obj_can_read (const char *pathname)
{
  return access (pathname, R_OK) == 0;
}

XTEXT_UNUSED const char *
xtext_obj_get_filename (xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx);
  XTEXT_HAND_IS_READABLE (xtext_ctx, NULL);
  const xtext_io_t *bio = &xtext_ctx->binary_file;
  return bio->pathname;
}

XTEXT_UNUSED const xtext_error_e
xtext_error_get_last (const xtext_ctx_t *xtext_ctx)
{
  return xtext_ctx->error_instatus;
}

XTEXT_UNUSED size_t
xtext_obj_get_size (xtext_ctx_t *xtext_ctx)
{
  XTEXT_HAND_IS_READABLE (xtext_ctx, 0);
  const xtext_info_t *xbin_info = &xtext_ctx->binary_info;
  return xbin_info->binary_size;
}

XTEXT_UNUSED size_t
xtext_obj_memory_size (xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx != NULL);
  XTEXT_HAND_IS_READABLE (xtext_ctx, 0);
  const xtext_map_t *heap_xbin_mmap = &xtext_ctx->binary_map;
  return heap_xbin_mmap->map_size;
}

/* Checks binary type and returns it */
static xtext_type_t
CheckMagic (const unsigned char magic_hdr[4])
{
  if (memcmp (magic_hdr, elf_signature, sizeof (elf_signature)) == 0)
    {
      /* Binary is a ELF file */
      return XTEXT_ELF_FORMAT;
    }
  return XTEXT_TYPE_UNKN;
}

/* Returns object type current loaded */
XTEXT_UNUSED xtext_type_t
xtext_obj_get_type (xtext_ctx_t *xtext_ctx)
{
  XTEXT_HAND_IS_READABLE (xtext_ctx, XTEXT_TYPE_UNKN);
  const xtext_map_t *heap_xbin_mmap = &xtext_ctx->binary_map;
  uint8_t *magic_hdr = XTEXT_MAKE_PTR (0, heap_xbin_mmap->map_start);
  return CheckMagic (magic_hdr);
}

const char *const xbin_types_str[];

XTEXT_UNUSED const char *
xtext_obj_type_to_str (xtext_type_t bin_type)
{
  if (bin_type > XTEXT_ELF_FORMAT)
    {
      bin_type = XTEXT_TYPE_UNKN;
    }
  return xbin_types_str[bin_type];
}

const char *const xbin_types_str[] = {
  // XTEXT_TYPE_UNKN
  "unknown (not recognized, a.k.a UNK)",
  // XTEXT_PE_FORMAT
  "portable executable (PE)",
  // XTEXT_ELF_FORMAT
  "executable and linkable format (ELF)", NULL
};

XTEXT_UNUSED bool
bin_load_file (const char *pathname, xtext_ctx_t *xtext_ctx)
{
#define SYNC_IO_OPERATIONS 1
  if (xtext_obj_can_read (pathname) == false)
    {
#if defined(__unix__)
      xtext_ctx->internal_error = errno;
#endif
      xtext_ctx->error_instatus = XTEXT_E_OPEN_FILE;
      return false;
    }
  /* If there's any file or memory allocated, clear everything now
   * before continue!
   */
  memset (xtext_ctx, 0, sizeof (xtext_ctx_t));
  xtext_io_t *xbin_contio = &xtext_ctx->binary_file;
#if defined(__unix__)
  xbin_contio->fd_flags =
#if SYNC_IO_OPERATIONS
      O_DSYNC |
#endif
      /* O_RDWR | O_WRONLY | */
      O_RDONLY;
#if _POSIX_C_SOURCE >= 200809L
  /* Opening current directory stored by process */
  native_fd_t openDir = xbin_contio->dir_fdesc
      = open (".", O_DIRECTORY | /* O_PATH */ O_RDONLY);

  if (openDir == -1)
    {
      xtext_ctx->internal_error = errno;
      xtext_ctx->error_instatus = XTEXT_E_OPEN_FILE;
      return false;
    }

  const native_fd_t open_fd = xbin_contio->object_nfd
      = openat (xbin_contio->dir_fdesc, pathname, xbin_contio->fd_flags);
  if (open_fd == -1)
    {
      xtext_ctx->internal_error = errno;
      xtext_ctx->error_instatus = XTEXT_E_OPEN_FILE;
      return false;
    }
  close (xbin_contio->dir_fdesc);
  xbin_contio->dir_fdesc = -1;
#else
  const native_fd_t open_fd = xbin_contio->object_nfd
      = open (pathname, xbin_contio->fd_flags);
#endif
#endif
  /* At this moment the file has been opened with success */
  xbin_contio->pathname = strdup (pathname);
  if (xbin_contio->pathname == NULL)
    {
      xtext_ctx->error_instatus = XTEXT_E_MALLOCATION_ERROR;
      return false;
    }
  xtext_ctx->error_instatus = XTEXT_E_OK;
  return true;
}

XTEXT_UNUSED bool
xtext_unload_file (xtext_ctx_t *xtext)
{
  assert (xtext != NULL);
#if defined(__unix__)
  /* Cleaning errno state */
  errno = 0;
  xtext_io_t *bio = &xtext->binary_file;
  native_fd_t bin_fd = bio->object_nfd;

  if (bio->pathname != NULL)
    {
      free (bio->pathname);
      bio->pathname = NULL;
    }
  if (bin_fd != -1)
    {
      close (bin_fd);
      /* Ensuring an invalid state after the file was closed */
      bio->object_nfd = bin_fd = -1;
    }
  if (errno != 0)
    {
      xtext->error_instatus = XTEXT_E_CLOSE_FILE;
      return false;
    }
#endif
  xtext->error_instatus = XTEXT_E_OK;
  return true;
}

static bool
unmap_file_mem (xtext_ctx_t *xtext_ctx)
{
  void *map_start;
  size_t map_size;
  xtext_map_t *map = &xtext_ctx->binary_map;

  map_start = map->map_start;
  map_size = map->map_size;
  assert (map_start != NULL);
  assert (map->map_end != 0);
  assert (map_size != 0);

#if defined(__unix__)
  if (munmap (map_start, map_size) != 0)
    {
      xtext_ctx->internal_error = errno;
      xtext_ctx->error_instatus = XTEXT_E_MUNMAP_FAILED;
    }
#endif
  map->map_start = NULL;
  map->map_end = map->map_size = 0;

  return true;
}

/* Returns the number of pages allocated (MEMORY / 1024) */
static bool
map_file_memory (xtext_ctx_t *xtext)
{
  size_t map_size;
  uint8_t *map_start;
  uintptr_t map_end;

  xtext_map_t *map = &xtext->binary_map;
  xtext_io_t *bio = &xtext->binary_file;
  xtext_info_t *xbin_info = &xtext->binary_info;

  size_t bin_filesize = xbin_info->binary_size;

/* Or something like: 1,048,576 * x */
#define MEBIBYTE(x) ((x)*1024 * 1024) // 1048576
  assert (bin_filesize < MEBIBYTE (124));
/* Mapping the file in memory */
#if defined(__unix__)
  const int32_t prot = PROT_READ | PROT_WRITE;
  const int32_t flags = /* MAP_SHARED */ MAP_PRIVATE;

  map_size = bin_filesize;
  map_start = mmap (NULL, map_size, prot, flags, bio->object_nfd, 0);

  if (map_start == MAP_FAILED)
    {
      xtext->error_instatus = XTEXT_E_MMAP_FAILED;
      return false;
    }

  map_end = (uintptr_t)map_start + map_size;
  map->map_start = map_start;
  map->map_end = map_end;
  map->map_size = map_size;
  /* Advise kernel about allocation purposes */
  madvise (map_start, map_size, MADV_SEQUENTIAL);
  /* Closing the file descriptor (is not more useful) */
  close (bio->object_nfd);
  bio->object_nfd = -1;
#endif
  return true;
}

XTEXT_UNUSED bool
xtext_parser (xtext_ctx_t *xtext_ctx)
{
  xtext_io_t *bio = &xtext_ctx->binary_file;
  xtext_info_t *xbin_info = &xtext_ctx->binary_info;

  if (xtext_ctx->error_instatus != XTEXT_E_OK)
    {
      return false;
    }

  if (xtext_obj_is_loaded (xtext_ctx) == true)
    {
      xtext_ctx->error_instatus = XTEXT_E_ALREADY_PARSED;
      return false;
    }
#if defined(__unix__)
  const native_fd_t fd = bio->object_nfd;
  if (fd == -1)
    {
      xtext_ctx->error_instatus = XTEXT_E_OPEN_FILE;
      return false;
    }
  errno = 0;
  struct stat fdStat;
  const int32_t statRet = fstat (fd, &fdStat);
  if (statRet == -1)
    {
      xtext_ctx->error_instatus = XTEXT_E_FSTAT_FAILED;
      return false;
    }
  const size_t binSize = fdStat.st_size;

  if (binSize == 0)
    {
      if (errno == 0)
        {
          xtext_ctx->error_instatus = XTEXT_E_IS_EMPTY;
          return false;
        }
      else
        {
          xtext_ctx->error_instatus = XTEXT_E_FSTAT_FAILED;
          return false;
        }
    }
  xbin_info->binary_size = binSize;

#endif
  /* Must be bigger than 0 */
  if (map_file_memory (xtext_ctx) == false)
    {
      if (xtext_ctx->error_instatus != XTEXT_E_OK)
        {
          return false;
        }
    }
  xtext_ctx->error_instatus = XTEXT_E_OK;
  return true;
}

static const char *const error_list_str[];

XTEXT_UNUSED const char *
xtext_error_to_str (xtext_error_e error_value)
{
  if (error_value > XTEXT_E_NOT_A_ELF)
    {
      error_value = XTEXT_E_NOT_A_ELF;
    }
  return error_list_str[error_value];
}

static const char *const error_list_str[] = {
  // XTEXT_E_OK
  "everything is ok",
  // XTEXT_E_OPEN_FILE
  "can't open the file, maybe the file not exist",
  // XTEXT_E_CLOSE_FILE
  "can't close the file, maybe has been deleted",
  // XTEXT_E_MALLOCATION_ERROR
  "a recently allocation using the malloc function has been failed",
  // XTEXT_E_IS_EMPTY
  "the supposed binary file is empty",
  // XTEXT_E_ALREADY_PARSED
  "the executable has been already parsed",
#if defined(__unix__)
  // XTEXT_E_FSTAT_FAILED
  "a recent call for function fstat has returned a fail value",
  // XTEXT_E_MMAP_FAILED
  "a recent call for mmap has failed",
  // XTEXT_E_MUNMAP_FAILED
  "a recent call for munmap has failed",
#endif
  // XTEXT_E_CANT_READ
  "a recently try to read a chunk of memory has been failed",
  // XTEXT_E_INVALID_FILE
  "an invalid file name was been passed",
  // XTEXT_E_NOT_A_ELF
  "object file isn't an ELF", NULL
};

static const char *cpu_endian_str[]
    = { "unknown CPU endianness", "2's complement, little endian",
        "2's complement, big endian", NULL };

XTEXT_UNUSED const char *
xtext_cpu_endian_to_str (cpu_endian_e cpu_endian)
{
  if (cpu_endian > CPUE_END_BIG)
    {
      cpu_endian = CPUE_END_UNKN;
    }
  return cpu_endian_str[cpu_endian];
}

/* Translate a elf_endian_e into a cpu_endian_e */
static cpu_endian_e
elf_cpu_endian (elf_endian_e elf_endian_bits)
{
  switch (elf_endian_bits)
    {
    default:
    case ELF_DATA_NONE:
      return CPUE_END_UNKN;
    case ELF_DATA_2LSB:
      return CPUE_END_LITTLE;
    case ELF_DATA_2MSB:
      return CPUE_END_BIG;
    }
}

/* Returns object CPU endianness */
XTEXT_UNUSED cpu_endian_e
xtext_obj_get_endian (xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx != NULL);
  cpu_endian_e cpu_endian = CPUE_END_LITTLE;
  const xtext_map_t *heap_xbin_mmap = &xtext_ctx->binary_map;
  const uint8_t *elfIdent = (const uint8_t *)XTEXT_MAKE_PTR (
      ELF_IDENT_OFF, heap_xbin_mmap->map_start);
  switch (xtext_obj_get_type (xtext_ctx))
    {
    case XTEXT_TYPE_UNKN:
    case XTEXT_ELF_FORMAT:
      cpu_endian = elf_cpu_endian ((elf_endian_e)elfIdent[5]);
      break;
    case XTEXT_PE_FORMAT:
      break;
    }

  return cpu_endian;
}

static const char *binary_class_str[]
    = { "unknown object class", "32 bits", "64 bits" };

XTEXT_UNUSED const char *
xtext_class_to_str (class_bits_e class_bits)
{
  if (class_bits > CLASS_64_BITS)
    {
      class_bits = CLASS_UNKNOWN;
    }
  return binary_class_str[class_bits];
}

static class_bits_e
elf_cl (elf_class_e elfc)
{
  switch (elfc)
    {
    default:
    case ELF_CLASS_32:
      return CLASS_32_BITS;
    case ELF_CLASS_64:
      return CLASS_64_BITS;
    }
}

XTEXT_UNUSED class_bits_e
xtext_obj_get_class (xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx != NULL);
  XTEXT_HAND_IS_READABLE (xtext_ctx, false);
  class_bits_e class_bits = CLASS_32_BITS;
  const xtext_map_t *heap_xbin_mmap = &xtext_ctx->binary_map;
  const uint8_t *elfIdent = (const uint8_t *)XTEXT_MAKE_PTR (
      ELF_IDENT_OFF, heap_xbin_mmap->map_start);

  switch (xtext_obj_get_type (xtext_ctx))
    {
    case XTEXT_ELF_FORMAT:
      class_bits = elf_cl ((elf_class_e)elfIdent[ELF_IDENT_CLASS]);
      break;
    case XTEXT_PE_FORMAT:
    case XTEXT_TYPE_UNKN:
      break;
    }
  return class_bits;
}

XTEXT_UNUSED bool
xtext_obj_class_is_32b (xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx != NULL);
  return xtext_obj_get_class (xtext_ctx) == CLASS_32_BITS;
}

XTEXT_UNUSED bool
etext_obj_class_is_64b (xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx != NULL);
  return xtext_obj_get_class (xtext_ctx) == CLASS_64_BITS;
}

XTEXT_UNUSED bool
xtext_obj_symhdr_foreach (xtext_ctx_t *xtext_ctx, cb_sym_hdr_t user_func,
                          elfsec_hdr_t *use_object)
{
  assert (xtext_ctx);
  XTEXT_HAND_IS_READABLE (xtext_ctx, false);
  if (xtext_obj_is_ELF (xtext_ctx) == false)
    {
      /* The file isn't an ELF format */
      xtext_ctx->error_instatus = XTEXT_E_NOT_A_ELF;
      return false;
    }

  uint64_t sec_off;
  uint16_t sec_size, sec_count;

  const class_bits_e class = xtext_obj_get_class (xtext_ctx);
  switch (class)
    {
    case CLASS_64_BITS:
      sec_off = ELF_SYMHDR64_OFF;
      sec_size = ELF_SECHDR64_SZ_OFF;
      sec_count = ELF_SECHDR64_COUNT_OFF;
    case CLASS_32_BITS:
    case CLASS_UNKNOWN:
    default:
      break;
    }

  xtext_map_t *heap_xbin_mmap = &xtext_ctx->binary_map;
  int_fast32_t offset_index = 0;

#define XTERN_COPY_PTR(dest, relative, map)                                     \
  memcpy (&(dest), XTEXT_MAKE_PTR (relative, map), sizeof (dest))

  XTERN_COPY_PTR (sec_off, sec_off, heap_xbin_mmap->map_start);
  XTERN_COPY_PTR (sec_size, sec_size, heap_xbin_mmap->map_start);
  XTERN_COPY_PTR (sec_count, sec_count, heap_xbin_mmap->map_start);
  for (; sec_count-- > 0;)
    {
      if (user_func (xtext_ctx,
                     (elfsec_hdr_t *)XTEXT_MAKE_PTR (
                         sec_off + sec_size * offset_index++,
                         heap_xbin_mmap->map_start),
                     use_object)
          == false)
        break;
    }
  return true;
}

static bool
elf_search_symbs_table (xtext_ctx_t *xtext_ctx,
                        const elfsec_hdr_t *func_object,
                        elfsec_hdr_t *user_object)
{

  const elf_sec_hdr64_t *elf_sec64
      = (const elf_sec_hdr64_t *)&func_object->header64;
  /* const elf_sec_hdr32_t *elfSecHdr32 = (const
   * elf_sec_hdr32_t*)&func_object->header32; */

  if (etext_obj_class_is_64b (xtext_ctx))
    {
      if (elf_sec64->secType == SHT_SYM_TAB)
        {
          memcpy (user_object, func_object, sizeof (elfsec_hdr_t));
        }
    }
  return true;
}

XTEXT_UNUSED bool
xtext_obj_symbols_foreach (xtext_ctx_t *xtext_ctx, cb_symb_t user_func,
                           object_symb_t *user_object)
{
  assert (xtext_ctx != NULL && user_func != NULL);

  elfsec_hdr_t symbol_table = { 0 };

  /*
      elfsec_hdr_t namesTable = {0};
      elfsec_hdr_t stringsTable = {0};
  */

  XTEXT_HAND_IS_READABLE (xtext_ctx, false);

  switch (xtext_obj_get_type (xtext_ctx))
    {
    case XTEXT_ELF_FORMAT:
      /* Search for symbol table inside the object */
      xtext_obj_symhdr_foreach (xtext_ctx, elf_search_symbs_table,
                                &symbol_table);
    /*
        xtext_obj_symhdr_foreach(xtext_ctx, ELFSearchForNamesTable,
       &namesTable); xtext_obj_symhdr_foreach(xtext_ctx,
       ELFSearchForNamesTable, &stringsTable);
    */
    case XTEXT_PE_FORMAT:
    case XTEXT_TYPE_UNKN:
      break;
    }

  return true;
}

XTEXT_UNUSED bool
xtext_obj_is_loaded (const xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx != NULL);
  const xtext_map_t *heap_xbin_mmap = &xtext_ctx->binary_map;
  return heap_xbin_mmap->map_start != NULL && heap_xbin_mmap->map_size > 0;
}

/* Sanitizer check if binary is an ELF format */
XTEXT_UNUSED bool
xtext_obj_is_ELF (xtext_ctx_t *xtext_ctx)
{
  assert (xtext_ctx != NULL);
  XTEXT_HAND_IS_READABLE (xtext_ctx, false);

  const xtext_map_t *heap_xbin_mmap = &xtext_ctx->binary_map;
  const uint8_t *elfIdent = (const uint8_t *)XTEXT_MAKE_PTR (
      ELF_IDENT_OFF, heap_xbin_mmap->map_start);
  uint8_t header_magicX[4];
  memcpy (header_magicX, elfIdent, sizeof (header_magicX));
  return CheckMagic (header_magicX) == XTEXT_ELF_FORMAT;
}

XTEXT_UNUSED bool
bin_finish (xtext_ctx_t *xtext)
{
  if (xtext_unload_file (xtext) != XTEXT_E_OK)
    {
      return false;
    }
  /* Unmapping memory region pages allocated for binary */
  unmap_file_mem (xtext);
  if (xtext->error_instatus == XTEXT_E_MUNMAP_FAILED)
    {
      return false;
    }
  /* Must be XTEXT_E_OK */
  const xtext_error_e error_instatus = xtext->error_instatus;
  memset (xtext, 0, sizeof (xtext_ctx_t));
  return error_instatus;
}
