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
