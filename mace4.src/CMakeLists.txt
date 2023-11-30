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

set(objects
    estack.c
    util.c
    print.c
    syms.c
    ground.c
    arithmetic.c
    select.c
    propagate.c
    mstate.c
    negpropindex.c
    negprop.c
    ordercells.c
    commandline.c
    msearch.c
)

add_library(mace4 STATIC ${objects})
target_link_libraries(mace4 ladr)

add_executable(mace4_exe mace4.c)
set_property(TARGET mace4_exe PROPERTY OUTPUT_NAME mace4)
target_link_libraries(mace4_exe mace4)
