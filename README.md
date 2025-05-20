# HarvesterHarvester

HarvesterHarvester (HH) was my first ever large C project. Additionally, being a reverse engineering project, I was really figuring out the specifications for it as I went along. As a result, the original HH is in desperate need of a rewrite. This is currently an on-going project, the end product of which will be released as v2.0.0.

This new version hopes to make HH more easily maintainable and extendible, with the hope being to apply the same processes used here for other, similar games in the future.

# Old README

A tool for harvesting data out of [_Harvester_'s (DigiFX Interactive, 1996)](https://en.wikipedia.org/wiki/Harvester_(video_game)) dat files.

This program is the first piece of a toolchain that I'm developing as I work to reverse engineering *Harvester*. The ultimate goal of this project is to eventually port the game to a more modern graphics framework (Raylib or OpenGL) just for fun/to teach myself the basics of reverse engineering. 

The program works in two steps: 
1. It reconstructs the original disk images that Harvester would've shipped with in 1996
   * _Note_: if you own the original game disks, this step is not needed.
   * See [USAGE.md](.github/USAGE.md) for more details.
2. It extracts the dat files contained within those new directories in order to give you full access to all of the game's files (~2GiB in total!).

### Future Projects

The next step for me in this project will be creating an asset viewer.

This will require me to reverse engineer all of the various, non-standardized file formats that DigiFX used when making the game, which will be essential for porting the game in the future.

So far, I've already been able to reverse engineer the `*.BM` files, which are used for all the static backgrounds in the games, for character portraits, the map, and more.  Unfortunately, I likely can't to share these files, at least not in bulk (see [below](#A-note-on-piracy)). I will, however, be sharing a small handful of these images as I show this project off online and I'll make sure to add a link here in the future if you wish to view those! 

## Installation
### Easiest Way
Navigate to the [releases](https://github.com/IcePanorama/HarvesterHarvester/releases) page and download the latest zip file.

Once downloaded, extract the zip file wherever you'd like and you're done! See [USAGE.md](.github/USAGE.md) for more details on how to run the program.

### Building from source (Linux/MacOS/Unix)
1) Install the following dependencies:
```
gcc make cmake mingw32-gcc clang-tools-extra valgrind doxygen
```
2) Clone the repo and cd into this new directory:
```bash
$ git clone https://github.com/IcePanorama/HarvesterHarvester/ && cd HarvesterHarvester
# ... or, alternatively ...
$ git clone git@github.com:IcePanorama/HarvesterHarvester.git && cd HarvesterHarvester
```
3) Create a build directory and cd into there:
```
$ mkdir build && cd build
```
3) Build using cmake and make:
```bash
$ cmake ..
$ make release # for *nix, x86_64, and i686 executables
# ... or, alternatively ...
$ make full    # for just the *nix executable
```
Running `make release` is strongly recommended as the build process for the release target makes use of every single dependency listed above, thus ensuring that you have all the correct packages installed on your machine.

Congrats! You're now all set to [use the program](.github/USAGE.md) or [start contributing](.github/CONTRIBUTING.md).

## A note on piracy

This program requires that you legally own a copy of _Harvester_ in order to source the necessary dat files. 

The GOG version (which this project was built around) [is available here for $5.99](https://www.gog.com/en/game/harvester), although it's worth noting that it does go on sale rather often.

[Alternatively, visit any of the retailers on this link if you'd rather shop elsewhere](https://isthereanydeal.com/game/harvester/info/).

This project in no way intends to encourage the piracy of DigiFX' software.
