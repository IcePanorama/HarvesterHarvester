# HarvesterHarvester (HH) Usage Guide

The following guide assumes you've already installed HH onto your computer. If this is not the case, then please see [README.md](../README.md#getting-hh) for installation instructions. The location of HH on your computer will hereafter be referred to as `HH_DIR`.

\[TODO: add note for Windows 10/11 users about making their HH directory case-sensitive for best results].

## Using HH With Modern Releases of *Harvester* (GOG/Steam)

HH requires *Harvester*'s .dat files to operate. The "install location" of *Harvester* (where these .dat files are stored) depends on your copy of the game. 

For users who purchased the game via GOG, the install location will be whatever location you specified during installation. If you're unsure where that is, please see [the following support article](https://support.gog.com/hc/en-us/articles/213039625-Where-is-my-game-installed?product=gog) for GOG's default installation folder location. Note for Windows users: `$HOME` in said article means your `C:\` directory.

For Steam users, the install location can be found by right-clicking on the game in your library, clicking "Properties", then "Installed Files", and lastly, "Browse...".

Now that've you've determined where Harvester is located on your computer, you can either:

1. copy `HARVEST.DAT`, `HARVEST3.DAT`, and `HARVEST4.DAT` to `<HH_DIR>/dat-files`; or
2. provide a path to those .dat files when you execute HH

The benefit of the first approach is that HH will automatically search the `dat-files` directory for those dat files, allowing you to simply double-click the HH executable in order to run it. Conversely, the latter approach requires you to execute HH via a command prompt or terminal.

**Note**: `HARVEST2.DAT` is not needed as its contents are contained within the other .dat files.

## Using HH With Physical Releases of *Harvester* (original CDs)

As noted in the [README.md](../README.md), HH works in two steps, the first of which reconstructs the original disk images that *Harvester* would've shipped with at the time of its release. If you still have the game disks, however, this first step is not needed.

At this point, you can execute HH via a command prompt or terminal by providing a path to each CD's `HARVEST.DAT`, `HARVEST2.DAT`, and `SOUND.DAT` files. HH will automatically detect that these files are internal dat files and extract them. Note that HH assumes that each .dat file's associated index file is also stored in the same directory.

Alternatively, you can place each disc's dat and index files into `<HH_DIR>/dat-files/DISK<?>`, where `<?>` corresponds to each disc's number and can be either `1`, `2`, or `3`. The benefit of this second approach is that HH can automatically search the `dat-files` directory for those dat/index files, saving you from typing out the paths directly.

If you do take this second approach, you'll be recreating the following structure in `dat-files`:

```
dat-files/
├── !!! DAT FILES GO HERE.txt
├── DISK1
│   ├── HARVEST.DAT
│   ├── HARVEST2.DAT
│   ├── SOUND.DAT
│   ├── INDEX.000
│   ├── INDEX.001
│   └── INDEX.002
├── DISK2
│   ├── HARVEST.DAT
│   ├── HARVEST2.DAT
│   ├── SOUND.DAT
│   ├── INDEX.000
│   ├── INDEX.001
│   └── INDEX.002
└── DISK3
    ├── HARVEST.DAT
    ├── HARVEST2.DAT
    ├── SOUND.DAT
    ├── INDEX.000
    ├── INDEX.001
    └── INDEX.002
```

In order for this to work, however, you MUST execute HH with the  `--skip-i9660-dats` flag. See [Advanced Usage](#advanced-usage) below for more details.

## Using HH as a Library

HH v2.0.0 has temporarily removed the static library targets from its releases. This functionality is expected to return in an upcoming minor update (that is, v2.x.0).

I first want to make all the ISO 9660 related functionalities its own separate library (upon which HH will then depend) before then releasing these new targets. What's more, I don't think that delaying the release of v2.0.0 for that to be done is a worthwhile endeavor.

Ultimately, I do still plan on working towards an asset viewer for Harvester--a program which will depend upon this library functionality--however, that won't be happening any time soon; I still need to finish reverse engineering all of the various file formats that Harvester uses first. In other words, I have plenty of time to come back to this later.

That being said, technologically-inclined readers could probably get a library target working with minimal effort. And if you need or want to do that for some project of yours, go for it. Just please don't submit issues related to HH's API until after this library update is completed. Or if you do, don't expect them to be immediately fixed.

## Using HH With Other ISO 9660 File Systems

When one provides HH with a file path that it doesn't recognize, it defaults to extracting this unknown entity as an ISO 9660 (I9660) file system. Therefore, HH can also be used like a generic I9660 file system extractor.

This functionality has been tested to work on \[TODO: list what things have been tested in this way].

To use HH in this capacity, simply provide a path to said file system as an argument:

\[TODO: EDIT THE FOLLOWING USAGE EXAMPLE TO MAKE SURE THIS IS ACTUAL CORRECT]

```
# Example:
$ ./HarvesterHarvester path/to/fs.bin # the extension can be anything, in fact.
```

**Note**: support for other file systems is currently experimental and not fully implemented. This is admittedly due to the fact that I need examples of different kinds of I9660 file systems other than ones with primary volume descriptor data in order to finalize these functionalities. If you run HH on something other than *Harvester*, and you get an error message, please open up [a new issue]()! Thanks!

\[TODO: add issues link above.]

## Advanced Usage

```
Usage:
  <HarvesterHarvester> [options] [path/to/file.dat ...]

Options:
  -h, --help                Print this help message
  -o <path>,--output <path> Set output directory to `<path>` (default: `./output/`)
  --skip-i9660-dats         Treat input like internal dat files.
  --skip-internal-dats      Skip the extraction of internal dat files
  -v, --version             Print version information
```

Where `HarvesterHarvester` is the appropriate executable for your system.

