//  Copyright (C) 2024  IcePanorama
//  This file is a part of HarvesterHarvester.
//  HarvesterHarvester is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by the
//  Free Software Foundation, either version 3 of the License, or (at your
//  option) any later version.
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//  more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <https://www.gnu.org/licenses/>.
#include "volume_descriptor.h"
#include "data_reader.h"
#include "errors.h"

#include <stdint.h>
#include <string.h>

void
create_volume_descriptor (volume_descriptor *vd, uint8_t type_code,
                          uint8_t version)
{
  vd->type_code = type_code;
  strncpy (vd->identifier, "CD001", sizeof (vd->identifier));
  vd->identifier[sizeof (vd->identifier) - 1] = '\0';
  vd->version = version;
}

void
print_volume_descriptor_header (volume_descriptor *vd)
{
  printf ("Volume descriptor type code: %02x\n", vd->type_code);
  printf ("Volume descriptor identifier: %s\n", vd->identifier);
  printf ("Volume descriptor version: %02x\n", vd->version);
}

void
print_volume_descriptor_data (volume_descriptor_data *vdd)
{
  printf ("System identifier: %s\n", vdd->system_identifier);
  printf ("Volume identifier: %s\n", vdd->volume_identifier);
  printf ("Vol space size: %08X\n", vdd->volume_space_size);
  printf ("Vol set size: %04X\n", vdd->volume_set_size);
  printf ("Vol sequence size: %04X\n", vdd->volume_sequence_number);
  printf ("Logical block size: %04X\n", vdd->logical_block_size);
  printf ("Path table size: %08X\n", vdd->path_table_size);
  printf ("Type-L path table location: %08X\n",
          vdd->type_l_path_table_location);
  printf ("Optional Type-L path table location: %08X\n",
          vdd->optional_type_l_path_table_location);
  printf ("Type-M path table location: %08X\n",
          vdd->type_m_path_table_location);
  printf ("Optional Type-M path table location: %08X\n",
          vdd->optional_type_m_path_table_location);
  printf ("Volume set identifier: %s\n", vdd->volume_set_identifier);
  printf ("Publisher identifier: %s\n", vdd->publisher_identifier);
  printf ("Data preparer identifier: %s\n", vdd->data_preparer_identifier);
  printf ("Application identifier: %s\n", vdd->application_identifier);
  printf ("Copyright file identifier: %s\n", vdd->copyright_file_identifier);
  printf ("Abstract file identifier: %s\n", vdd->abstract_file_identifier);
  printf ("Bibliographic file identifier: %s\n",
          vdd->bibliographic_file_identifier);
  printf ("Volume creation date and time: ");
  print_dec_datetime (vdd->volume_creation_date_and_time);
  printf ("Volume modification date and time: ");
  print_dec_datetime (vdd->volume_modification_date_and_time);
  printf ("Volume expiration date and time: ");
  print_dec_datetime (vdd->volume_expiration_date_and_time);
  printf ("Volume effective date and time: ");
  print_dec_datetime (vdd->volume_effective_date_and_time);
  printf ("File structure version: %02X\n", vdd->file_structure_version);
  printf ("DAT file creation software identifier: %s\n",
          vdd->DAT_file_creation_software_identifier);
  printf ("DAT file creation software version number: %s\n",
          vdd->DAT_file_creation_software_version_number);
}

int8_t
process_volume_descriptor_header (FILE *fptr, volume_descriptor *vd)
{
  uint8_t descriptor_type;
  if (read_single_uint8 (fptr, &descriptor_type) != 0)
    return HH_FREAD_ERROR;

  fseek (fptr, 5, SEEK_CUR);

  uint8_t descriptor_ver;
  if (read_single_uint8 (fptr, &descriptor_ver) != 0)
    return HH_FREAD_ERROR;

  create_volume_descriptor (vd, descriptor_type, descriptor_ver);

  return 0;
}

int8_t
process_volume_descriptor_data (FILE *fptr, volume_descriptor_data *vdd)
{
  fseek (fptr, 1, SEEK_CUR); // Unused byte

  if ((read_string (fptr, vdd->system_identifier, SYSTEM_IDENTIFIER_LEN) != 0)
      || (read_string (fptr, vdd->volume_identifier, VOLUME_IDENTIFIER_LEN)
          != 0))
    {
      return HH_FREAD_ERROR;
    }

  fseek (fptr, 8, SEEK_CUR); // Unused field

  if (read_both_endian_data_uint32 (fptr, &vdd->volume_space_size) != 0)
    return HH_FREAD_ERROR;

  fseek (fptr, 32, SEEK_CUR); // Unused field

  /* clang-format off */
  if ((read_both_endian_data_uint16 (fptr, &vdd->volume_set_size) != 0)
      || (read_both_endian_data_uint16 (fptr, &vdd->volume_sequence_number) != 0)
      || (read_both_endian_data_uint16 (fptr, &vdd->logical_block_size) != 0))
  {
    return HH_FREAD_ERROR;
  }
  /* clang-format on */

  if ((read_both_endian_data_uint32 (fptr, &vdd->path_table_size) != 0)
      || (read_little_endian_data_uint32_t (fptr,
                                            &vdd->type_l_path_table_location)
          != 0)
      || (read_little_endian_data_uint32_t (
              fptr, &vdd->optional_type_l_path_table_location)
          != 0)
      || (read_little_endian_data_uint32_t (fptr,
                                            &vdd->type_m_path_table_location)
          != 0)
      || (read_little_endian_data_uint32_t (
              fptr, &vdd->optional_type_m_path_table_location)
          != 0))
    {
      return HH_FREAD_ERROR;
    }

  if (read_array_uint8 (fptr, vdd->root_directory_entry,
                        ROOT_DIRECTORY_ENTRY_LEN)
      != 0)
    {
      return HH_FREAD_ERROR;
    }

  /* clang-format off */
  if ((read_string (fptr, vdd->volume_set_identifier, VOLUME_SET_IDENTIFIER_LEN) != 0)
      || (read_string (fptr, vdd->publisher_identifier, PUBLISHER_IDENTIFIER_LEN) != 0)
      || (read_string (fptr, vdd->data_preparer_identifier, DATA_PREPARER_IDENTIFIER_LEN) != 0)
      || (read_string (fptr, vdd->application_identifier, APPLICATION_IDENTIFIER_LEN) != 0)
      || (read_string (fptr, vdd->copyright_file_identifier, COPYRIGHT_FILE_IDENTIFIER_LEN) != 0)
      || (read_string (fptr, vdd->abstract_file_identifier, ABSTRACT_FILE_IDENTIFIER_LEN) != 0)
      || (read_string (fptr, vdd->bibliographic_file_identifier, BIBLIOGRAPHIC_FILE_IDENTIFIER_LEN) != 0))
  {
    return HH_FREAD_ERROR;
  }
  /* clang-format on */

  if ((read_dec_datetime (fptr, &vdd->volume_creation_date_and_time) != 0)
      || (read_dec_datetime (fptr, &vdd->volume_modification_date_and_time)
          != 0)
      || (read_dec_datetime (fptr, &vdd->volume_expiration_date_and_time) != 0)
      || (read_dec_datetime (fptr, &vdd->volume_effective_date_and_time) != 0))
    {
      return HH_FREAD_ERROR;
    }

  if (read_single_uint8 (fptr, &vdd->file_structure_version) != 0)
    return HH_FREAD_ERROR;

  fseek (fptr, 1, SEEK_CUR); // Unused byte

  read_string (fptr, vdd->DAT_file_creation_software_identifier,
               DAT_FILE_CREATION_SOFTWARE_IDENTIFIER_LEN);

  fseek (fptr, 1, SEEK_CUR); // Unused byte

  read_string (fptr, vdd->DAT_file_creation_software_version_number,
               DAT_FILE_CREATION_SOFTWARE_VERSION_NUMBER_LEN);

  fseek (fptr, 1, SEEK_CUR); // Unused byte

  fseek (fptr, 0x1EC, SEEK_CUR); // Unknown data (492 bytes)

  fseek (fptr, 0x28D, SEEK_CUR); // Reserved by ISO

  return 0;
}
