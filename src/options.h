#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <stdbool.h>

extern bool debug_mode;
extern bool separate_outputs;

void handle_command_line_args (int argc, char **argv);

#endif
