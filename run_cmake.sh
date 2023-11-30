#!/bin/bash
# Copyright (C) 2023 LaiTeP and contributors
#
# This file is part of the LADR Deduction Library.
#
# The LADR Deduction Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License, version 2.
#
# The LADR Deduction Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# the LADR Deduction Library; if not, write to the Free Software Foundation,
# Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
#

# https://stackoverflow.com/a/246128
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd $SCRIPT_DIR

if [ -d bin ]; then
    rm -r bin
    mkdir bin
    echo "# bin" > bin/README.md
fi

if [ -d build ]; then
    rm -r build
fi

GENERATOR_FLAG=""
if command -v cmake &> /dev/null && \
  cmake --help 2>&1 | grep -q "build.ninja" && \
  command -v ninja &> /dev/null; \
then
    echo "Ninja found. Setting -GNinja."
    GENERATOR_FLAG="-GNinja"
fi

mkdir build && cd build
cmake $GENERATOR_FLAG -DCMAKE_INSTALL_PREFIX=$SCRIPT_DIR ..
cmake --build .
cmake --install .
ctest
cd ..
