# make_releases - a script for generating every possible version of
# HarvesterHarvester.
# This script basically just runs each `X.cmake` toolchain file in `./.cmake`.
# It's implementation is quite specific to my current setup, however, that
# should be fine as I'll likely be the only person to ever actually make
# release builds.
#! /usr/bin/env bash
BUILD_DIR=build
HH_OUTPUT_DIR=output

function setup ()
{
	rm -rf ${BUILD_DIR} &> /dev/null
	mkdir ${BUILD_DIR} &> /dev/null
	cd ${BUILD_DIR}
}

function tear_down ()
{
	make full
	cd ..
}

function build ()
{
	setup
	if [[ -z $2 ]]; then
		cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$1 ..
	else
		cmake -DCMAKE_C_COMPILER=$2 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$1 ..
	fi
	tear_down
}

function run_tests ()
{
	python ./tools/batch-md5 output
	python ./tools/verify-chksums ./.github/full_output_checksums.txt checksums.txt
}

rm -rf ${HH_OUTPUT_DIR} &> /dev/null
if [[ -d ${BUILD_DIR} ]]; then
	cd ${BUILD_DIR}
	make clean
	cd ..
fi

build ../.cmake/linux_amd64.cmake
run_tests

build ../.cmake/linux_i686.cmake
run_tests

build ../.cmake/windows_amd64.cmake x86_64-w64-mingw32-gcc
build ../.cmake/windows_i686.cmake i686-w64-mingw32-gcc
