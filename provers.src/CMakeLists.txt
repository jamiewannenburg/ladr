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

set(sources
    actions.c
    demodulate.c
    foffer.c
    forward_subsume.c
    giv_select.c
    index_lits.c
    pred_elim.c
    provers.c
    search.c
    semantics.c
    unfold.c
    utilities.c
    white_black.c
)

add_executable(prover9 prover9.c ${sources})
target_link_libraries(prover9 ladr m)

add_executable(fof-prover9 fof-prover9.c ${sources})
target_link_libraries(fof-prover9 ladr m)

add_executable(ladr_to_tptp ladr_to_tptp.c ${sources})
target_link_libraries(ladr_to_tptp ladr m)

add_executable(tptp_to_ladr tptp_to_ladr.c ${sources})
target_link_libraries(tptp_to_ladr ladr m)

add_executable(autosketches4 autosketches4.c ${sources})
target_link_libraries(autosketches4 ladr m)

add_executable(newauto newauto.c ${sources})
target_link_libraries(newauto ladr m)

add_executable(newsax newsax.c ${sources})
target_link_libraries(newsax ladr m)
