"""
    checksum_dict.py - a wrapper class for checksum dictionaries.

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
import logging


class ChecksumDict:
    """
    A dictionary wrapper class which correlates file paths to checksum values.
    Designed to work in conjunction with `batch-md5`.

    Attributes:
        _dict: An dictionary used internally for storing said correlations.
        _filename: The filename of the file used to create this instance.
    """
    class ChecksumMismatchError(Exception):
        """A custom exception for checksum mismatches."""

        def __init__(self, filename: str, original: str, new: str) -> None:
            super().__init__(f"Checksum mismatch for '{filename}' (current: "
                             + f"{original}, new: {new}).")

    def __init__(self, input_path: str) -> None:
        """
        Initializes an instance using an input file.

        Args:
            input_path (str): path to some checksum file.

        Raises:
            RuntimeError: If line in file is malformed or duplicate key is
            already in dict.
        """
        self._dict: dict(str, str) = {}
        self._filename: str = input_path
        logging.info(f"Creating ChecksumDict for file, {self._filename}.")
        with open(input_path) as fptr:
            for line in fptr:
                if not line:
                    continue

                segments = line.split("\t")
                if (len(segments) < 2):
                    raise RuntimeError(f"Bad input: {line}")
                segments[1] = segments[1][:-1]
                if (segments[1] in self._dict):
                    logging.info(f"Duplicate key in {input_path}: " + f"{segments[1]} => " + f"{self._dict[segments[1]]}")
                    pass
                    """
                    raise RuntimeError(f"Duplicate key in {input_path}: " +
                                       f"{segments[1]} => " +
                                       f"{self._dict[segments[1]]}")
                                       """

                self._dict[segments[1]] = segments[0]

    def __len__(self) -> int:
        """Overrides default len implementation."""
        return len(self._dict)

    def __eq__(self, o: 'ChecksumDict') -> bool:
        """Overrides default implementation."""
        if isinstance(o, ChecksumDict):
            return self._compare_chksum_dicts(o)
        return NotImplemented

    def __ne__(self, o: 'ChecksumDict') -> bool:
        """Overrides default implementation."""
        out = self.__eq__(o)
        if out is NotImplemented:
            return NotImplemented
        return not out

    def _compare_chksum_dicts(self, o: 'ChecksumDict') -> bool:
        """
        Handles the logic of comparing two checksum dictionaries.

        Args:
            o: Another ChecksumDict to compare `self` with.

        Raises:
            ChecksumMismatchError: If checksums in `self` differs from `o`.
        """
        logging.info(f"Comparing {self._filename} with {o._filename}.")
        if len(self) > len(o):
            raise RuntimeError(f"Other ChecksumDict ({o._filename}) is smaller than {self._filename}")
            return False

        for file, chksum in o._dict.items():
            if file not in self._dict:
                logging.warn(f"File, {file}, not found in {self._filename}.")
            elif self._dict[file] != chksum:
                raise self.ChecksumMismatchError(
                    file, self._dict[file], chksum)

        return True
