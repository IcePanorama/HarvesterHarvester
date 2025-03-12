#include "iso_9660/path_table_entry.h"

#include "binary_reader.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  Processes all the entries in the path table that `input_fptr` currently
 *  points to, placing each entry in `pts` and updating `pt_max_size` as
 *  needed.
 *  @see `ISO9660PathTableEntry_t`
 */
static int8_t process_pt_entries (FILE *input_fptr,
                                  ISO9660PathTableEntry_t **pts,
                                  size_t *pt_max_size, uint32_t pt_end);

static int read_pt_from_file (FILE *fptr, ISO9660PathTableEntry_t *pt);
static void print_pt (ISO9660PathTableEntry_t *pt);

/**
 *  Calculates the length of a primary volume descriptor's volume identifier.
 *  This string isn't NULL-terminated, therefore this function looks for the
 *  first space character.
 *  @returns the position of the first space character in `volume_identifier`
 */
static int calculate_pvd_vol_id_len (const char *volume_identifier);

int
read_path_tables_from_file (FILE input_fptr[static 1],
                            ISO9660PathTableEntry_t *pt_list[static 1],
                            size_t ptable_entries_len[static 1],
                            uint32_t pt_start_loc, uint32_t pt_size)
{
  uint64_t pt_end = pt_start_loc + pt_size;
  if (fseek (input_fptr, pt_start_loc, SEEK_SET) != 0)
    {
      fprintf (
          stderr,
          "ERROR: failed to seek to root directory path table (0x%08X).\n",
          pt_start_loc);
      goto clean_up;
    }

  if (process_pt_entries (input_fptr, pt_list, ptable_entries_len, pt_end) != 0)
    goto clean_up;

  return 0;
clean_up:
  return -1;
}

int
alloc_pt_entries_array (ISO9660PathTableEntry_t **pts, size_t size)
{
  (*pts) = calloc (size, sizeof (ISO9660PathTableEntry_t));
  if ((*pts) == NULL)
    {
      fprintf (stderr,
               "ERROR: Failed to alloc path table array of size, %ld\n", size);
      return -1;
    }

  return 0;
}

int8_t
process_pt_entries (FILE *input_fptr, ISO9660PathTableEntry_t **pts,
                    size_t *pt_max_size, uint32_t pt_end)
{
  size_t i = 0;
  uint32_t pos = 0;
  do
    {
      if (i >= (*pt_max_size))
        {
          (*pt_max_size) *= 2;
          ISO9660PathTableEntry_t *tmp = realloc (
              (*pts), sizeof (ISO9660PathTableEntry_t) * (*pt_max_size));
          if (tmp == NULL)
            {
              fprintf (stderr,
                       "ERROR: Failed to grow path table array to size, %ld\n",
                       (*pt_max_size));
              return -1;
            }
          (*pts) = tmp;
        }

      if (read_pt_from_file (input_fptr, &(*pts)[i]) != 0)
        {
          return -1;
        }
      print_pt (&(*pts)[i]);
      i++;

      pos = (uint32_t)(ftell (input_fptr));
    }
  while (pos != pt_end);

  (*pt_max_size) = i;

  return 0;
}

int
read_pt_from_file (FILE *fptr, ISO9660PathTableEntry_t *pt)
{
  if ((br_read_u8_from_file (fptr, &pt->directory_identifier_length) != 0)
      || (br_read_u8_from_file (fptr, &pt->extended_attribute_record_length)
          != 0)
      || (br_read_le_u32_from_file (fptr, &pt->extent_location) != 0)
      || (br_read_le_u16_from_file (fptr, &pt->parent_directory_number) != 0)
      || (br_read_str_from_file (fptr, pt->directory_identifier,
                                 pt->directory_identifier_length)
          != 0))
    {
      return -1;
    }

  if ((pt->directory_identifier_length % 2) != 0)
    {
      if (fseek (fptr, 1, SEEK_CUR) != 0)
        {
          fprintf (stderr,
                   "ERROR: failed to seek past padding after path table.\n");
          return -1;
        }
    }

  return 0;
}

void
print_pt (ISO9660PathTableEntry_t *pt)
{
  printf ("%.*s - Ext. attrib. len: %d, Loc: %d, Parent: %d\n",
          pt->directory_identifier_length, pt->directory_identifier,
          pt->extended_attribute_record_length, pt->extent_location,
          pt->parent_directory_number);
}

int
build_paths_from_pt_list (ISO9660PathTableEntry_t pt_list[static 1],
                          char *path_list[static 1], size_t list_len,
                          const char pvd_vol_id[static 1],
                          const char output_dir[static 1])
{
  const uint_least8_t pvd_vol_id_len = calculate_pvd_vol_id_len (pvd_vol_id);

  for (size_t i = list_len - 1; i > 0; i--)
    {
      ISO9660PathTableEntry_t *curr = &pt_list[i];
      size_t path_len = curr->directory_identifier_length + 1;

      char *path = calloc (path_len, sizeof (char));
      if (path == NULL)
        {
          fprintf (stderr, "ERROR: Unable to alloc path string of size, %ld\n",
                   path_len);
          u_free_partial_list_elements ((void **)path_list, i + 1, list_len);
          return -1;
        }

      strncpy (path, curr->directory_identifier,
               curr->directory_identifier_length);
      path[path_len - 1] = '\0';

      // Recursively build path string by following `curr`'s parent dir.
      ISO9660PathTableEntry_t *tmp = curr;
      do
        {

          // `parent_directory_number` is 1-indexed.
          tmp = &pt_list[tmp->parent_directory_number - 1];

          /*
           *  `directory_identifier` is NOT NULL-terminated, hence the need for
           *  passing its length as well.
           */
          if (u_prepend_path_str (&path, tmp->directory_identifier,
                                  tmp->directory_identifier_length)
              != 0)
            {

              u_free_partial_list_elements ((void **)path_list, i + 1,
                                            list_len);
              free (path);
              return -1;
            }
        }
      while (tmp->parent_directory_number > 1);

      u_prepend_path_str (&path, pvd_vol_id, pvd_vol_id_len);
      u_prepend_path_str (&path, output_dir, strlen (output_dir));
      path_list[i] = path;
      printf ("Path: %s\n", path_list[i]);
    }

  // handle root dir, 2 for path separator & NULL-terminator
  char *path = calloc (pvd_vol_id_len + strlen (output_dir) + 2, sizeof (char));
  if (path == NULL)
    {
      u_free_partial_list_elements ((void **)path_list, 1, list_len);
      return -1;
    }

  strncpy (path, pvd_vol_id, pvd_vol_id_len);
  u_prepend_path_str (&path, output_dir, strlen (output_dir));
  path_list[0] = path;
  printf ("Path: %s\n", path_list[0]);

  return 0;
}

int
calculate_pvd_vol_id_len (const char *volume_identifier)
{
  size_t i;
  for (i = 0; i < 32; i++)
    {
      if (volume_identifier[i] == ' ')
        break;
    }

  return i;
}
