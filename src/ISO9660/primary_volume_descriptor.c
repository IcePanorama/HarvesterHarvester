#include "ISO9660/primary_volume_descriptor.h"
#include "ISO9660/directory_record.h"
#include "binary_reader.h"

static int read_pvd_date_time_from_file (FILE *fptr,
                                         ISO9660PrimaryVolumeDateTime_t *dt);
static void print_pvd_date_time (const char *date_time_identifier,
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

void
print_pvd_data (ISO9660PrimaryVolumeDescriptorData_t *pvdd)
{
  printf ("- System identifier: %.*s\n", 32, pvdd->system_identifier);
  printf ("- Volume identifier: %.*s\n", 32, pvdd->volume_identifier);
  printf ("- Volume space size: %d\n", pvdd->volume_space_size);
  printf ("- Volume set size: %d\n", pvdd->volume_set_size);
  printf ("- Volume sequence number: %d\n", pvdd->volume_sequence_number);
  printf ("- Logical block size: %d\n", pvdd->logical_block_size);

  printf ("- Path table size: %d\n", pvdd->path_table_size);
  printf ("- Location of type-l path table: 0x%08X\n",
          pvdd->type_l_path_table_location);
  printf ("- Location of optional type-l path table: 0x%08X\n",
          pvdd->optional_type_l_path_table_location);
  printf ("- Location of type-m path table: 0x%08X\n",
          pvdd->type_m_path_table_location);
  printf ("- Location of optional type-m path table: 0x%08X\n",
          pvdd->optional_type_m_path_table_location);

  puts ("- Root directory entry:");
  print_dir_rec (&pvdd->root_directory_entry);

  printf ("- Volume set identifier: %.*s\n", 128, pvdd->volume_set_identifier);
  printf ("- Publisher identifier: %.*s\n", 128, pvdd->publisher_identifier);
  printf ("- Data preparer identifier: %.*s\n", 128,
          pvdd->data_preparer_identifier);
  printf ("- Application identifier: %.*s\n", 128,
          pvdd->application_identifier);
  printf ("- Copyright file identifier: %.*s\n", 37,
          pvdd->copyright_file_identifier);
  printf ("- Abstract file identifier: %.*s\n", 37,
          pvdd->abstract_file_identifier);
  printf ("- Bibliographic file identifier: %.*s\n", 37,
          pvdd->bibliographic_file_identifier);

  print_pvd_date_time ("- Volume creation date time",
                       &pvdd->volume_creation_date_time);
  print_pvd_date_time ("- Volume modification date time",
                       &pvdd->volume_modification_date_time);
  print_pvd_date_time ("- Volume expiration date time",
                       &pvdd->volume_expiration_date_time);
  print_pvd_date_time ("- Volume effective date time",
                       &pvdd->volume_effective_date_time);

  printf ("- File structure version: %d\n", pvdd->file_structure_version);
}

void
print_pvd_date_time (const char *date_time_identifier,
                     ISO9660PrimaryVolumeDateTime_t *dt)
{
  printf ("%s: %.4s-%.2s-%.2s %.2s:%.2s:%.2s.%.2s (GMT%+02d)\n",
          date_time_identifier, dt->year, dt->month, dt->day, dt->hour,
          dt->minute, dt->second, dt->hundredths_of_a_second,
          (-48 + dt->timezone) >> 2);
}
