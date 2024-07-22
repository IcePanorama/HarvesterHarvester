# HarvesterHarvester
A tool for harvesting data out of [_Harvester_'s (DigiFX Interactive, 1996)](https://en.wikipedia.org/wiki/Harvester_(video_game)) DAT files.

## Installation
### Easiest Way
Navigate to the [releases](https://github.com/IcePanorama/HarvesterHarvester/releases) page and download the latest zip file. Once downloaded, simply extract the zip file and run the appropriate executable given your operating system/architecture.

If you're on a modern Windows machine and you're unsure of which file to run, your best bet is likely the one ending in `x86_64`; that executable should run on most Windows machines made in the past 15 years.

**NOTE**: _if you're reading this in the present, you'll notice that there currently aren't any available releases. I'm writing this README in advance -- this project isn't quite done yet, sorry!_

### Building from source (Linux/MacOS/Unix)
1) Install the following dependencies:
```
gcc make mingw32-gcc clang-tools-extra valgrind doxygen
```
2) Clone the repo:
```bash
$ git clone https://github.com/IcePanorama/HarvesterHarvester/ && cd HarvesterHarvester
```
3) Build using make:
```bash
$ make release # for *nix, x86_64, and i686 executables
# ... or, alternatively ...
$ make full    # for just the *nix executable
```
Running `make release` is strongly recommended as the build process for the release target makes use of every single dependency listed above, thus ensuring that you have all the correct packages installed on your machine.

## Usage
1) Install Harvester onto your machine if you haven't already done so.
2) Copy the following files from your harvester installation to the `dat-files/` folder:
```
HARVEST.DAT
HARVEST3.DAT
HARVEST4.DAT
```
4) Run the `HarvesterHarvester` executable.

## To Do
+ Handle SIGINT/Windows-equivalent of SIGINT gracefully
    + Currently, if a user presses `CTRL+C` while the program is executing, this causes a memory leak.
        + To see this in action, run `make test` and then press `CTRL+C`.
    + An ideal solution would free any outstanding, allocated memory and close open file handlers before exiting
    + Ideally, this solution should also be cross-platform, if at all possible.
+ Create a guide for building the project from source for Windows users. 

## A note on piracy

This program requires that you legally own a copy of _Harvester_ in order to source the necessary .DAT files. 

The GOG version (which this project was built around) [is available here for $5.99](https://www.gog.com/en/game/harvester), although it's worth noting that it does go on sale rather often.

[Alternatively, visit any of the retailers on this link if you'd rather shop elsewhere](https://isthereanydeal.com/game/harvester/info/).

This project in no way intends to encourage the piracy of DigiFX' software.
