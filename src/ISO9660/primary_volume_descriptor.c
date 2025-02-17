#include "ISO9660/primary_volume_descriptor.h"
#include "ISO9660/directory_record.h"
#include "binary_reader.h"

static int read_pvd_date_time_from_file (FILE *fptr,
                                         ISO9660PrimaryVolumeDateTime_t *dt);

int
read_pvd_data_from_file (FILE *fptr,
                         ISO9660PrimaryVolumeDescriptorData_t *pvdd)
{
  if (fseek (fptr, 1, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if ((br_read_str_from_file (fptr, pvdd->system_identifier, 32) != 0)
      || (br_read_str_from_file (fptr, pvdd->volume_identifier, 32)))
    return -1;

  if (fseek (fptr, 8, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  if (br_read_le_be_u32_from_file (fptr, &pvdd->volume_space_size) != 0)
    return -1;

  if (fseek (fptr, 32, SEEK_CUR) != 0) // Unused
    goto fseek_err;

  /* clang-format off */
  if ((br_read_le_be_u16_from_file (fptr, &pvdd->volume_set_size) != 0)
      || (br_read_le_be_u16_from_file (fptr, &pvdd->volume_sequence_number) != 0)
      || (br_read_le_be_u16_from_file (fptr, &pvdd->logical_block_size) != 0)
      || (br_read_le_be_u32_from_file (fptr, &pvdd->path_table_size) != 0)
      || (br_read_le_u32_from_file (fptr, &pvdd->type_l_path_table_location) != 0)
      || (br_read_le_u32_from_file (fptr, &pvdd->optional_type_l_path_table_location) != 0)
      || (br_read_be_u32_from_file (fptr, &pvdd->type_m_path_table_location) != 0)
      || (br_read_be_u32_from_file (fptr, &pvdd->optional_type_m_path_table_location) != 0)
      || (read_dir_rec_from_file (fptr, &pvdd->root_directory_entry) != 0)
      || (br_read_str_from_file (fptr, pvdd->volume_set_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->publisher_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->data_preparer_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->application_identifier, 128) != 0)
      || (br_read_str_from_file (fptr, pvdd->copyright_file_identifier, 37) != 0)
      || (br_read_str_from_file (fptr, pvdd->abstract_file_identifier, 37) != 0)
      || (br_read_str_from_file (fptr, pvdd->bibliographic_file_identifier, 37) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_creation_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_modification_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_expiration_date_time) != 0)
      || (read_pvd_date_time_from_file (fptr, &pvdd->volume_effective_date_time) != 0)
      || (br_read_u8_from_file (fptr, &pvdd->file_structure_version) != 0)
      || (br_read_u8_array_from_file(fptr, pvdd->application_used_data, 512) != 0))
    return -1;
  /* clang-format on */

  return 0;
fseek_err:
  fprintf (stderr, "ERROR: failed to seek past unused byte(s) in primary "
                   "volume descriptor data.\n");
  return -1;
}

int
read_pvd_date_time_from_file (FILE *fptr, ISO9660PrimaryVolumeDateTime_t *dt)
{
  if ((br_read_str_from_file (fptr, dt->year, 4) != 0)
      || (br_read_str_from_file (fptr, dt->month, 2) != 0)
      || (br_read_str_from_file (fptr, dt->day, 2) != 0)
      || (br_read_str_from_file (fptr, dt->hour, 2) != 0)
      || (br_read_str_from_file (fptr, dt->minute, 2) != 0)
      || (br_read_str_from_file (fptr, dt->second, 2) != 0)
      || (br_read_str_from_file (fptr, dt->hundredths_of_a_second, 2) != 0)
      || (br_read_u8_from_file (fptr, &dt->timezone) != 0))
    {
      return -1;
    }

  return 0;
}
