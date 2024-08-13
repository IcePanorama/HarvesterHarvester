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
#include "log.h"
#include "errors.h"

#include <stdarg.h>
#include <stdio.h>

/**
 *  Outputs hex data from a given `buffer` to stdout. Formats said data to
 *  add spaces between bytes, tabs after after every four bytes, and a new line
 *  after every 16 bytes.
 *
 *  TODO: These values are currently hardcoded in, but they likely should be
 *  moved to options.h in the future.
 */
void
print_hex_data (unsigned char *buffer, const uint8_t BUFFER_LEN)
{
  for (uint8_t i = 0; i < BUFFER_LEN; i++)
    {
      printf (" %02x", buffer[i]);
      if ((i != 0) && (i % 4 == 3))
        {
          printf ("\t");
        }
      if ((i != 0) && (i % 16 == 15))
        {
          puts ("");
        }
    }
  puts ("");
}

#define BYTES_TO_READ 32
/**
 *  As this function can safely fail without affecting normal operations, this
 *  doesn't return any errors, despite possibly having a fread error during
 *  normal usage.
 */
void
print_some_data_from_file (FILE *fptr)
{
  unsigned char buffer[BYTES_TO_READ];
  size_t bytes_read = fread (buffer, sizeof (buffer[0]), BYTES_TO_READ, fptr);
  if (bytes_read != BYTES_TO_READ)
    {
      handle_fread_error (bytes_read, BYTES_TO_READ);
      return;
    }

  print_hex_data (buffer, BYTES_TO_READ);
  fseek (fptr, -BYTES_TO_READ, SEEK_CUR);
}

// TODO: add an command-line arg to output to a log file or to run silently.
void
hh_log (hh_log_level lvl, const char *fmt, ...)
{
  const char *program_tag = "[HarvesterHarvester]";
  FILE *output_stream = stdout;

  switch (lvl)
    {
    case HH_INFO:
      fprintf (stdout, "%s[INFO] ", program_tag);
      output_stream = stdout;
      break;
    case HH_WARNING:
      fprintf (stderr, "%s[WARNING] ", program_tag);
      output_stream = stderr;
      break;
    case HH_ERROR:
      fprintf (stderr, "%s[ERROR] ", program_tag);
      output_stream = stderr;
      break;
    default:
      hh_log (HH_ERROR, "Unrecognized hh_log_level, %d", lvl);
      return;
    }

  va_list args;
  va_start (args, fmt);
  vfprintf (output_stream, fmt, args);
  va_end (args);
  fprintf (output_stream, "\n");
}
