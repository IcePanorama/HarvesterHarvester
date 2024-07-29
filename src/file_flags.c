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
#include "file_flags.h"

#include <stdio.h>

file_flags
create_file_flags (void)
{
  file_flags ff;
  ff.hidden = false;
  ff.subdirectory = false;
  ff.associated_file = false;
  ff.extended_attribute_record_contains_owner_and_group_permissions = false;
  ff.final_directory_record = false;
  return ff;
}

void
print_file_flags (file_flags *ff)
{
  printf ("\tHidden: %d\n", ff->hidden);
  printf ("\tSubdirectory: %d\n", ff->subdirectory);
  printf ("\tAssociated file: %d\n", ff->associated_file);
  printf (
      "\tExtended attrib. record contains owner and group permissions: %d\n",
      ff->extended_attribute_record_contains_owner_and_group_permissions);
  printf ("\tFinal directory record: %d\n", ff->final_directory_record);
}
