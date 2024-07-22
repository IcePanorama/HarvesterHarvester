# HarvesterHarvester
A tool for harvesting data out of Harvester's (DigiFX Interactive, 1996) DAT files.

# To Do
+ Handle SIGINT/Windows equivalent gracefully
    + Currently, if a user presses `CTRL+C` while the program is executing, this causes a memory leak.
        + To see this in action, run `make test` and then press `CTRL+C`.
    + An ideal solution would free any outstanding, allocated memory and close open file handlers before exiting
    + Ideally, this solution should also be cross-platform, if at all possible.
