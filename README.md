# HarvesterHarvester
A tool for harvesting data out of Harvester's (DigiFX Interactive, 1996) DAT files.

# To Do
+ Handle SIGINT/Windows equivalent gracefully
    + Currently, if a user presses `CTRL+C` while the program is executing, this causes a memory leak.
    + An idea solution would free any currently-outstanding alloc'd memory and close file handlers before exiting
    + Ideally, this solution should also be cross-platform.
