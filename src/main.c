#include <stdio.h>
#include <stdlib.h>

void improper_usage_error (void);
void fopen_error (char *filename);

int
main (int argc, char **argv)
{
  unsigned char buffer[16];

  if (argc < 2)
    improper_usage_error ();

  FILE *fptr = fopen (argv[1], "rb");
  if (fptr == NULL)
    fopen_error (argv[1]);

  fread (buffer, sizeof (buffer), 1, fptr);

  for (int i = 0; i < 16; i++)
    {
      printf (" %02x", buffer[i]);
      if ((i != 0) && (i % 4 == 3))
        {
          printf ("\t");
        }
    }
  puts ("");

  fclose (fptr);
  return 0;
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
