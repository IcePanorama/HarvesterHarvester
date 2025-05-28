/**
 *  harvester_harvester/options.h - options for using HH.
 *
 *  Copyright (C) 2024-2025  IcePanorama
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef _HARVESTER_HARVESTER_OPTIONS_H_
#define _HARVESTER_HARVESTER_OPTIONS_H_

typedef struct HHOptions_s
{
  enum _HHOptProcMode_e
  {
    _HHPM_NORMAL_PROCESSING,
    _HHPM_SKIP_INTERNAL_DATS,
    _HHPM_SKIP_I9660_DATS
  } processing_mode;
} HHOptions_t;

#endif /* _HARVESTER_HARVESTER_OPTIONS_H_ */
