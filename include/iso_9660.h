#ifndef _ISO_9660_FILESYSTEM_H_
#define _ISO_9660_FILESYSTEM_H_

#include <stdint.h>

/** @see: https://wiki.osdev.org/ISO_9660#Directories */
typedef struct Iso9660DirectoryRecord_s
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
    uint8_t timezone_offset;
  } recording_date_time;

  uint8_t file_flags;
  uint8_t file_unit_size; //!< if recorded in interleaved mode, else 0.
  uint8_t interleave_gap_size; //!< if recorded in interleaved mode, else 0.
  /** "the volume that this extent is recorded on." */
  uint16_t volume_sequence_number;
  uint8_t file_identifier_length;

  /**
   *  Variable length file identifier.
   *  Length = `file_identifier_length` + ";1" (2) + '\0'.
   *
   *  @see: `file_identifier_length`.
   */
  char file_identifier[UINT8_MAX + 3];
} Iso9660DirectoryRecord_t;

typedef struct Iso9660PrimaryVolumeDateTime_s
{
  char year[4];
  char month[2];
  char day[2];
  char hour[2];
  char minute[2];
  char second[2];
  char hundredths_of_a_second[2];
  /** "offset from GMT in 15 minute intervals, [from] -48 ... to 52." */
  uint8_t time_zone_offset;
} Iso9660PrimaryVolumeDateTime_t;

/** @see: https://wiki.osdev.org/ISO_9660 */
typedef struct Iso9660FileSystem_s
{
  /** @see: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Type_Codes */
  enum VolumeDescriptorTypeCode_e
  {
    VDTC_BOOT_RECORD,
    VDTC_PRIMARY_VOLUME,
    VDTC_SUPPLEMENTARY_VOL,
    VDTC_VOL_PARTITION,
    VDTC_VOL_DESC_SET_TERMINATOR = 255
  } volume_desc_type_code;

  char volume_identifier[5];  //!< Always `CD001`.
  uint8_t volume_desc_version_num;

  /** @see: https://wiki.osdev.org/ISO_9660#Volume_Descriptors */
  union VolumeDescriptorData_u
  {
    /** @see: https://wiki.osdev.org/ISO_9660#The_Boot_Record */
    struct BootRecordVolumeDescriptorData_s
    {
      char boot_system_identifier[32];
      char boot_identifier[32];
      uint8_t boot_system_data[1977]; //!< "Custom - used by the boot system."
    } boot_vol_desc;

    /** @see: https://wiki.osdev.org/ISO_9660#The_Primary_Volume_Descriptor */
    struct PrimaryVolumeDescriptorData_s
    {
      char system_identifier[32];
      char volume_identifier[32];

      uint32_t volume_space_size;
      uint16_t volume_set_size;
      uint16_t volume_sequence_number;
      uint16_t logical_block_size;
      uint32_t path_table_size;
      uint32_t type_l_path_table_location;
      uint32_t optional_type_l_path_table_location;
      uint32_t type_m_path_table_location;
      uint32_t optional_type_m_path_table_location;

      Iso9660DirectoryRecord_t root_directory_entry;

      char volume_set_identifier[128];
      char publisher_identifier[128];
      char data_preparer_identifier[128];
      char application_identifier[128];
      char copyright_file_identifier[37];
      char abstract_file_identifier[37];
      char bibliographic_file_identifier[37];

      Iso9660PrimaryVolumeDateTime_t volume_creation_date_time;
      Iso9660PrimaryVolumeDateTime_t volume_modification_date_time;
      Iso9660PrimaryVolumeDateTime_t volume_expiration_date_time;
      Iso9660PrimaryVolumeDateTime_t volume_effective_date_time;

      uint8_t file_structure_version; //!< Always `0x01`.
      uint8_t application_used_data[512];
    } primary_vol_desc;

    /**
     * Volume Descriptor Set Terminator data "does not define bytes 7-2047 of
     * its Volume Descriptor."
     *  @see: https://wiki.osdev.org/ISO_9660#Volume_Descriptor_Set_Terminator
     *  TODO: decide: is it even worth creating a struct for this then?
     */
  } volume_desc_data;
} Iso9660FileSystem_t;

#endif /* _ISO_9660_FILESYSTEM_H_ */
