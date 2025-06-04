"""
    batch-md5 - calculates an md5 checksum for every file in a given directory
    and outputs the results to a plain text file.

    Copyright (C) 2025  IcePanorama

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""
import sys
import glob
import hashlib

output_filename: str = "checksums.txt"

if __name__ == "__main__":
    class InvalidUsageError(Exception):
        def __init__(self) -> None:
            super().__init__(
                f"Invalid usage. View usage: `python {sys.argv[0]} -h`")

    def handle_command_line_args(arg_list: list[str]) -> None:
        global output_filename
        i: int = 0
        while i < len(arg_list):
            arg: str = arg_list[i]

            if (arg == "-h" or arg == "--help"):
                print("Usage:")
                print(f"  python {sys.argv[0]} [options] path/to/some/dir\n")

                opts_fmt: str = "  {:<13} {}"
                print("Options:")
                print(opts_fmt.format("-h, --help", "Print this help message"))
                print(opts_fmt.format("-o, --output",
                      "Set output file name (default: checksums.txt)"))
                print(opts_fmt.format("-v, --version",
                      "Print version information"))
                exit(0)
            elif (arg == "-o" or arg == "--output"):
                if (i + 1) >= len(arg_list):
                    raise InvalidUsageError
                output_filename = arg_list[i + 1]
                i += 1
            else:
                break
            i += 1

    if len(sys.argv) < 2:
        raise InvalidUsageError
    else:
        handle_command_line_args(sys.argv[1:])

    # Assume last arg is input dir path
    input_dir: str = sys.argv[-1]
    if input_dir[-1] != '/':
        input_dir += '/'

    md5_map: dict[str, str] = {}
    for filename in glob.glob(input_dir + "**/**.**", recursive=True):
        print("Calculating checksum for " + filename)
        with open(filename, "rb") as f:
            assert (filename not in md5_map)
            md5_map[filename] = hashlib.file_digest(f, "md5").hexdigest()

    print("Writing checksums to " + output_filename)
    with open(output_filename, "w") as f:
        for k in md5_map.keys():
            f.write(f"{md5_map[k]}\t{k}\n")

    print("Done!")
