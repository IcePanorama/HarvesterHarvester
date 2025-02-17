#ifndef _ISO_9660_DIRECTORY_RECORD_
#define _ISO_9660_DIRECTORY_RECORD_

#include <stdint.h>
#include <stdio.h>

/** @see: https://wiki.osdev.org/ISO_9660#Directories */
typedef struct ISO9660DirectoryRecord_s
{
  uint8_t dir_rec_length;
  uint8_t extended_attrib_rec_length;
  uint32_t extent_location;
  uint32_t extent_size; //!< Also called its "data length".

  struct DirectoryRecordDateTime_s
  {
    uint8_t year; //!< number of years since 1900.
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    /** "Offset from GMT in 15 minute intervals from -48 ... to +52." */
    uint8_t timezone;
  } recording_date_time;

  uint8_t file_flags;
  /** if recorded in interleaved mode, else 0. */
  uint8_t file_unit_size;
  /** if recorded in interleaved mode, else 0. */
  uint8_t interleave_gap_size;
  /** "the volume that this extent is recorded on." */
  uint16_t volume_sequence_number;
  uint8_t file_identifier_length;

  /** Variable length file identifier. @see: `file_identifier_length`. */
  char file_identifier[UINT8_MAX];
} ISO9660DirectoryRecord_t;

/**
 *  Reads a directory record from file.
 *  @returns zero on success, non-zero on failure.
 *  @see `ISO9660DirectoryRecord_t`.
 */
int read_dir_rec_from_file (FILE *fptr, ISO9660DirectoryRecord_t *dr);

#endif /* _ISO_9660_DIRECTORY_RECORD_ */
