#include <stdio.h>
#include <stdlib.h>

#define BYTES_TO_READ 32

FILE *setup_extractor (int argc, char **argv);
void improper_usage_error (void);
void fopen_error (char *filename);
void process_DAT_file (FILE *fptr);

int
main (int argc, char **argv)
{
  FILE *fptr = setup_extractor (argc, argv);

  process_DAT_file (fptr);

  fclose (fptr);
  return 0;
}

FILE *
setup_extractor (int argc, char **argv)
{
  if (argc < 2)
    improper_usage_error ();

  FILE *fptr = fopen (argv[1], "rb");
  if (fptr == NULL)
    fopen_error (argv[1]);

  return fptr;
}

void
improper_usage_error (void)
{
  const char *EXECUTABLE_NAME = "main";

  puts ("ERROR: Improper usage!");
  printf ("try: $./%s HARVEST.DAT\n", EXECUTABLE_NAME);

  exit (1);
}

void
fopen_error (char *filename)
{
  printf ("ERROR: Unable to open file, %s.\n", filename);
  exit (1);
}

void
process_DAT_file (FILE *fptr)
{
  unsigned char buffer[BYTES_TO_READ];

  fread (buffer, sizeof (buffer), 1, fptr);

  for (int i = 0; i < BYTES_TO_READ; i++)
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
