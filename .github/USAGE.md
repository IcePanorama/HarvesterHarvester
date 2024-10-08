# HarvesterHarvester (HH) Usage Guide

The following document will attempt to give an in-depth guide on how to use HarvesterHarvester for a wide variety of possible use cases. These use cases include:
+ [Usage for users who own the game through GOG/Steam(\*)](#Using-HarvesterHarvester-with-Modern-Releases)
+ [Usage for users who own the game on CDs](#Using-HarvesterHarvester-With-Original-CDs)
+ [Advanced usage](#Advanced-Usage)
+ [Usage as a library](#Usage-as-a-library)
+ [Troubleshooting](#Troubleshooting)

\**I'm assuming here that the GOG and Steam releases are similar. If they aren't, please feel free to submit an [issue](https://github.com/IcePanorama/HarvesterHarvester/issues/).*

## Using HarvesterHarvester With Modern Releases

If you haven't done so already, install the game onto your local machine.

Once installed, navigate to the location where you installed Harvester. For GOG users, this'll be whatever location you specified during installation. For Steam users, you can easily find out where this location is by right-clicking on the game in your Steam library, then clicking `Properties -> Installed Files -> Browse...`.

You should find something similar to the picture below.

![Capture](https://github.com/user-attachments/assets/8adf89ef-8f06-4d4b-80be-2103f55a00f0)

From here, copy `HARVEST.DAT`, `HARVEST3.DAT`, and `HARVEST4.DAT` to `[wherever you installed HH]/dat-files`. `HARVEST2.DAT` is not needed as its contents are actually contained inside the other dat files.

Lastly, run HH:

```
HarvesterHarvester_x86_64.exe # 64-bit Windows machines, the default if you're not sure
HarvesterHarvester_i686.exe # 32-bit Windows machines
./HarvesterHarvester # *nix machines
```

## Using HarvesterHarvester With Original CDs

HH works in two steps. First, it reconstructs the original disk images that Harvester would've shipped with in 1996 and then second, it extracts the dat files contained within those new directories in order to give you full access to all of the game's files.

For users who already own the CDs, this first step is unnecessary, and so we'll be skipping it using a command-line argument (the `-i` flag).

_Full disclosure_: this isn't super elegant at the moment. I've been working under the assumption that CD users will be a small minority. Currently, all methods of working with CDs requires the contents of the disks to be copied somewhere onto your machine. I apologise in advance for the inconvenience, this should be fixed in v1.1.0 of HH.

By default, HH will try to process Harvester's 3 game disks by looking for a subdirectory named `output` in the HH installation directory. This `output` directory is expected contain three subdirectories: `DISK1`, `DISK2`, and `DISK3`.

Below gives a rough outline of what this should look like.

```
.
├── HarvesterHarvester
├── HarvesterHarvester_i686.exe
├── HarvesterHarvester_x86_64.exe
├── internal-dat-file-paths.txt
├── output
│   ├── DISK1
│   │   ├── ...
│   ├── DISK2
│   │   ├── ...
│   └── DISK3
│   │   ├── ...
├── README.md
```

There are three ways to approach using HH with CDs:

### 1. Recreate the Expected Hierarchy

The easiest solution is to recreate this hierarchy in the HH installation folder and to copy the contents of each CD to `DISK1`, `DISK2`, and `DISK3`.

The `DISK#` subdirectories don't actually have to correspond to their correct disk (e.g. copying the first disk to `DISK1`). These directories just need to exist.

At this point, you would just run HH with the `-i` flag (for internal dat file extraction only) via a command prompt/terminal.

```
HarvesterHarvester_x86_64.exe -i # 64-bit Windows machines, the default if you're not sure
HarvesterHarvester_i686.exe -i # 32-bit Windows machines
./HarvesterHarvester -i # *nix machines
```

### 2. Updating internal-dat-file-paths.txt and Using an Existing Local Installation

If you've already copied your disks to some place on your hard drive, you can update `internal-dat-file-paths.txt` as needed and then specify that directory using a command-line argument.

To update `internal-dat-file-paths.txt`, open the file in a text editor and change all occurrences of `DISK1`, `DISK2`, and `DISK3` to the names of those actual subdirectories. Don't touch any other part of this file. This  is my really cheap way of handling file associations needed for internal dat file extraction. :)

Once that's done, open up a command prompt/terminal wherever HH is installed and type:

```
HarvesterHarvester_x86_64.exe -i -o path\to\your\dir # 64-bit Windows machines, the default if you're not sure
HarvesterHarvester_i686.exe -i -o path\to\your\dir # 32-bit Windows machines
./HarvesterHarvester -i -o path/to/your/dir # *nix machines
```

### 3. For Those Who Haven’t Yet Copied Their Game Files

If you haven't already copied your disks to your hard drive, you can do so now, by creating a folder with three subdirectories, `DISK1`, `DISK2`, and `DISK3`.

Then, run HH by executing one of the following commands in a command-prompt/terminal:

```
HarvesterHarvester_x86_64.exe -i -o path\to\your\dir # 64-bit Windows machines, the default if you're not sure
HarvesterHarvester_i686.exe -i -o path\to\your\dir # 32-bit Windows machines
./HarvesterHarvester -i -o path/to/your/dir # *nix machines
```

## Advanced Usage

```
Usage:
	HarvesterHarvester_x86_64.exe [options] [path/to/dat_file.dat] # 64-bit Windows machines
	HarvesterHarvester_i686.exe [options] [path/to/dat_file.dat] # 32-bit Windows machines
	./HarvesterHarvester [options] [path/to/dat_file.dat] # *nix machines

Command-line arguments:
	--debug: enables debug mode, skips extracting large files. Useful for developers.
	--help: displays list of command line arguments.
	--version: prints out the version number.
	-d [path/to/some-dir]: specify the input directory for batch processing dat files. (Default=./dat-files/)
	-e: don't extract internal dat files.
	-i: extract internal dat files only.
	-o [path/to/some-dir]: extract dat files to the given directory/look here for internal dat files.

Optional:
	path/to/dat_file.dat: specify the path to a dat file to be processed.
```

## Usage as a library
Usage of HH as a library is still somewhat experimental. As time goes on, and I use it in future projects, its interface may very well change.

To get started, please see the [bin](https://github.com/IcePanorama/HarvesterHarvester/tree/main/bin) directory for a copy of the static library file and its headers.

`HarvesterHarvester/hh.h` is the main interface through which you should interact with HH. That being said, the other header files are also included as they may prove useful in future projects.

## Troubleshooting

### "Unexpected volume descriptor type code. Expected 0x01, got 0x83."
Full error message:
```bash
[HarvesterHarvester][ERROR]Unexpected volume descriptor type code. Expected 0x01, got 0x83.
```

This error likely arose either from you running HH on HARVEST2.DAT or from you running HH on batch processing mode (its default behavior) with HARVEST2.DAT in your batch processing input folder (default = ./dat-files/). It is unnecessary to use HARVEST2.DAT while running HH as a version of HARVEST2.DAT is included inside of each of the three dat files. These other versions of HARVEST2.DAT will automatically be processed during the second stage of execution.

If you come across a similar error not related to HARVEST2.DAT, the first recommend course of action would be to reinstall Harvester and then recopy all of its dat files to said input folder. If the issue still persists thereafter, [please open up a new issue here](https://github.com/IcePanorama/HarvesterHarvester/issues) or leave a comment on a pre-existing issue describing your particular situation.
