#ifndef _ISO_9660_DIRECTORY_RECORD_FILE_FLAGS_H_
#define _ISO_9660_DIRECTORY_RECORD_FILE_FLAGS_H_

#include <stdint.h>

/**
 *  File flag bits.
 *  See: https://wiki.osdev.org/ISO_9660#Directories.
 */
#define _FF_HIDDEN_FILE_BIT (0)
#define _FF_IS_DIRECTORY_BIT (1)
#define _FF_IS_ASSOC_FILE_BIT (2)
#define _FF_FMT_IN_EAR_BIT (3) // EAR: Extended attribute record
#define _FF_PERMS_IN_EAR_BIT (4)
#define _FF_NOT_FINAL_DIR_BIT (7)

typedef uint8_t _FileFlags_t;

#endif /* _ISO_9660_DIRECTORY_RECORD_FILE_FLAGS_H_ */
