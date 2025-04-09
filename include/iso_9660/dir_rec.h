#ifndef _ISO_9660_DIRECTORY_RECORD_H_
#define _ISO_9660_DIRECTORY_RECORD_H_

#include <stdint.h>
#include <stdio.h>

/** See:: https://wiki.osdev.org/ISO_9660#Directories */
typedef struct _DirRec_s
{
  uint8_t len;
  uint8_t extended_attrib_rec_len;
  uint32_t extent_loc;
  uint32_t extent_size; // Also called its "data length".

  struct _DRDateTime_s
  {
    uint8_t year; // number of years since 1900.
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    /** "Offset from GMT in 15 minute intervals from -48 ... to +52." */
    uint8_t timezone;
  } recording_date_time;

  uint8_t file_flags;     // See: https://wiki.osdev.org/ISO_9660#Directories.
  uint8_t file_unit_size; // if recorded in interleaved mode, else 0.
  uint8_t interleave_gap_size; // if recorded in interleaved mode, else 0.
  uint16_t vol_seq_num;        // "the volume that this extent is recorded on."
  uint8_t file_id_len;

  /** Variable length file identifier. See:: `file_id_len`. */
  char file_id[UINT8_MAX];
} _DirRec_t;

int _dr_init (_DirRec_t dr[static 1], FILE input_fptr[static 1]);
void _dr_print (_DirRec_t dr[static 1]);
int _dr_dynamic_init (_DirRec_t *dr_list[static 1], size_t list_cap[static 1],
                      size_t list_len[static 1], uint16_t lb_size,
                      FILE input_fptr[static 1]);
/** Param:  lb_size logical block size. */
int _dr_extract (_DirRec_t dr[static 1], size_t lb_size,
                 FILE input_fptr[static 1], char path[static 1]);

#endif /* _ISO_9660_DIRECTORY_RECORD_H_ */
