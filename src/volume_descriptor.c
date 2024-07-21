#include "volume_descriptor.h"

#include <stdio.h>
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
