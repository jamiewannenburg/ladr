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

add_executable(latfilter latfilter.c)
target_link_libraries(latfilter ladr)

add_executable(olfilter olfilter.c)
target_link_libraries(olfilter ladr)

add_executable(clausefilter clausefilter.c)
target_link_libraries(clausefilter ladr)

add_executable(idfilter idfilter.c)
target_link_libraries(idfilter ladr)

add_executable(renamer renamer.c)
target_link_libraries(renamer ladr)

add_executable(unfast unfast.c)
target_link_libraries(unfast ladr)

add_executable(clausetester clausetester.c)
target_link_libraries(clausetester ladr)

add_executable(rewriter rewriter.c)
target_link_libraries(rewriter ladr)

add_executable(isofilter0 isofilter0.c)
target_link_libraries(isofilter0 ladr)

add_executable(isofilter isofilter.c)
target_link_libraries(isofilter ladr)

add_executable(isofilter2 isofilter2.c)
target_link_libraries(isofilter2 ladr)

add_executable(dprofiles dprofiles.c)
target_link_libraries(dprofiles ladr)

add_executable(interpfilter interpfilter.c)
target_link_libraries(interpfilter ladr)

add_executable(upper-covers upper-covers.c)
target_link_libraries(upper-covers ladr)

add_executable(miniscope miniscope.c)
target_link_libraries(miniscope ladr)

add_executable(interpformat interpformat.c)
target_link_libraries(interpformat ladr)

add_executable(prooftrans prooftrans.c)
target_link_libraries(prooftrans ladr)

add_executable(mirror-flip mirror-flip.c)
target_link_libraries(mirror-flip ladr)

add_executable(perm3 perm3.c)
target_link_libraries(perm3 ladr)

add_executable(sigtest sigtest.c)
target_link_libraries(sigtest ladr)

add_executable(directproof directproof.c)
target_link_libraries(directproof ladr)

add_executable(test_clause_eval test_clause_eval.c)
target_link_libraries(test_clause_eval ladr)

add_executable(test_complex test_complex.c)
target_link_libraries(test_complex ladr)

add_executable(complex complex.c)
target_link_libraries(complex ladr)

add_executable(gen_trc_defs gen_trc_defs.c)
target_link_libraries(gen_trc_defs ladr)
