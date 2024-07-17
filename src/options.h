#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>

extern bool debug_mode;
extern char *current_disk_name;
extern bool OPT_USE_DEF_INPUT_DIR;

void handle_command_line_args (int argc, char **argv);

#endif
