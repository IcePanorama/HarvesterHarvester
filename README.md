# HarvesterHarvester

**HarvesterHarvester** (HH) is a tool for "harvesting" data from the .dat files of [DigiFX Interactive's 1996 game, *Harvester*](https://en.wikipedia.org/wiki/Harvester_(video_game)) (along with other file systems too)!

Intended to be the first piece in [a toolchain for reverse engineering *Harvester*](https://segfaulteddreams.neocities.org/projects/openharvester/), HH first reconstructs the original disk images that the game would've shipped with during its initial release before then extracting data from the .dat files contained within that new file system. In total, HH restores approximately 2.1 GiB of data.

\[TODO: double check that these links are good.\]

**Related tools**:

+ BMtoBMP: a tool for converting *Harvester*'s BM/PAL files into bitmap images. ([Homepage](https://segfaulteddreams.neocities.org/projects/bmtobmp/)/[GitHub](https://github.com/IcePanorama/BMtoBMP))
+ CMPto???: a tool for converting *Harvester*'s CMP files into some regular sound format. \[COMING SOON!\]

**New in v2.0.0** is the ability to apply these same processes to other, similar file systems as well! \[TODO: ADD NOTE ABOUT WHAT OTHER KINDS OF FILE SYSTEMS THIS TOOL HAS BEEN TESTED ON]

## Getting HH

The simpliest way to try HH for yourself is to navigate to [the releases page](https://github.com/IcePanorama/HarvesterHarvester/releases/latest) and download the correct zip file for your system. Once downloaded, simply extract the zip file wherever you'd like and you're ready to start *harvesting*. See [USAGE.md](.github/USAGE.md) for more details on how to run HH itself.

If you're unsure which zip file to use on your system, `HarvesterHarvester-<?>-Windows-amd64.zip` will likely be what you're looking for. If not, see below:

### Which release version is right for me?

```
# Windows
HarvesterHarvester-<?>-Windows-amd64 # 64-bit Windows (amd64/x86_64/x64)
HarvesterHarvester-<?>-Windows-i686  # 32-bit Windows (i686/x86)

# Linux/Unix/MacOS
HarvesterHarvester-<?>-Linux-amd64   # 64-bit Linux   (amd64/x86_64/x64)
HarvesterHarvester-<?>-Linux-i686    # 32-bit Linux   (i686/x86)
```

**NOTE**: I am currently unable to test whether MacOS support works. If you run into issues on MacOS, please open up a new [issue](https://github.com/IcePanorama/HarvesterHarvester/issues) provided one doesn't already exist! Thanks!

Prior to release, HH has been tested as working on Fedora Linux 42, Windows 10, and Windows 98. Continued support to all of these platforms, however, is not guaranteed.

### Building from Source

1. Install the following dependencies:

```
gcc make cmake        # Required
clang-format valgrind # Optional, but strongly recommended for developers
```

2. Download a copy of the source code:

```
$ git clone https://github.com/IcePanorama/HarvesterHarvester/ && cd HarvesterHarvester
# Or, alternatively ...
$ git clone git@github.com:IcePanorama/HarvesterHarvester.git && cd HarvesterHarvester
```

3. Use CMake to generate the necessary build files:

```
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=<?> -DCMAKE_TOOLCHAIN_FILE=<?> ..
```

Where `CMAKE_BUILD_TYPE` is either `Release` or `Debug` and `CMAKE_TOOLCHAIN_FILE` is a path to one of the many toolchain files in [the cmake directory](./.cmake).

**Note**: you may be required to define `CMAKE_C_COMPILER` while attempting to build a Windows executable on Linux (and, potentially, vice versa).

4. Build the final executable with `make`:

```
$ make
# Or, alternatively:
$ make full # Also checks for memory leaks via Valgrind
```

## Piracy Notice

HH requires that you supply your own .dat files for use.

The GOG version around which this project was built [is available here for $5.99](https://www.gog.com/en/game/harvester), although it is worth noting that it does go on sale rather often. Alternatively, visit any of the retailers [listed here](https://isthereanydeal.com/game/harvester/info/) if you'd rather shop elsewhere.

This project in no way intends to encourage the piracy of DigiFX Interactive's software.

## License

HarvesterHarvester is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

