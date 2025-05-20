#ifndef _HARVESTER_HARVESTER_OPTIONS_H_
#define _HARVESTER_HARVESTER_OPTIONS_H_

#include <stdbool.h>

typedef struct HHOptions_s
{
  enum
  {
    _HHPM_NORMAL_PROCESSING,
    _HHPM_SKIP_INTERNAL_DATS,
    _HHPM_SKIP_I9660_DATS
  } processing_mode;
} HHOptions_t;

#endif /* _HARVESTER_HARVESTER_OPTIONS_H_ */
