/* clang-format off */
#include <stdio.h>
#include <string.h>

#include "volume_descriptor.h"
/* clang-format on */

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
  printf ("%02x - %s (ver: %02x)\n", vd->type_code, vd->identifier,
          vd->version);
}
