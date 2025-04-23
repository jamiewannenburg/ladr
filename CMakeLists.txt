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

cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 11)

project(ladr)

option(LADR "Build the LADR static library" ON)
option(MACE4 "Build the mace4 binaries" ON)
option(PROVER9 "Build the prover9 binaries" ON)
option(APPS "Build the LADR applications binaries" ON)
option(TEST_CPP "Build the test_cpp binary" OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    message(
        "Changing the default CMAKE_INSTALL_PREFIX (${CMAKE_INSTALL_PREFIX})"
    )
    set(CMAKE_INSTALL_PREFIX
        "${CMAKE_CURRENT_LIST_DIR}"
        CACHE PATH "Install in the ladr folder by default" FORCE
    )
endif()

message("CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")

include(CTest)

if(LADR)
    message("Building ladr")
    add_subdirectory("ladr")
endif()

if(MACE4)
    message("Building mace4")
    add_subdirectory("mace4.src")
    install(TARGETS mace4_exe DESTINATION bin)
    add_test(NAME mace_test1
             COMMAND ${CMAKE_INSTALL_PREFIX}/bin/mace4 -v0 -f
                     ${CMAKE_CURRENT_LIST_DIR}/mace4.examples/group2.in
    )
    set_property(
        TEST mace_test1 PROPERTY PASS_REGULAR_EXPRESSION "For domain size 6"
                                 "Exiting with 1 model"
    )
    add_test(NAME mace_test2
             COMMAND ${CMAKE_INSTALL_PREFIX}/bin/mace4 -n3 -m -1 -f
                     ${CMAKE_CURRENT_LIST_DIR}/apps.examples/qg.in
    )
    set_property(
        TEST mace_test2 PROPERTY PASS_REGULAR_EXPRESSION
                                 "Exiting with 8 models" "all_models"
    )
endif()

if(PROVER9)
    message("Building prover9")
    add_subdirectory("provers.src")
    install(
        TARGETS prover9
                fof-prover9
                ladr_to_tptp
                tptp_to_ladr
                autosketches4
                newauto
                newsax
        DESTINATION bin
    )
    add_test(NAME prover_test1
             COMMAND ${CMAKE_INSTALL_PREFIX}/bin/prover9 -f
                     ${CMAKE_CURRENT_LIST_DIR}/prover9.examples/x2.in
    )
    set_property(
        TEST prover_test1 PROPERTY PASS_REGULAR_EXPRESSION "THEOREM PROVED"
                                   "Exiting with 1 proof" "max_proofs"
    )
endif()

if(APPS)
    message("Building apps")
    add_subdirectory("apps.src")
    install(
        TARGETS latfilter
                olfilter
                clausefilter
                idfilter
                renamer
                unfast
                clausetester
                rewriter
                isofilter0
                isofilter
                isofilter2
                dprofiles
                interpfilter
                upper-covers
                miniscope
                interpformat
                prooftrans
                mirror-flip
                perm3
                sigtest
                directproof
                test_clause_eval
                test_complex
                complex
                gen_trc_defs
        DESTINATION bin
    )
    install(DIRECTORY utilities/ DESTINATION bin)
    add_test(
        NAME prooftrans_test1
        COMMAND
            bash -c
            "${CMAKE_INSTALL_PREFIX}/bin/prover9 -f ${CMAKE_CURRENT_LIST_DIR}/prover9.examples/x2.in | ${CMAKE_INSTALL_PREFIX}/bin/prooftrans parents_only"
    )
    set_property(
        TEST prooftrans_test1 PROPERTY PASS_REGULAR_EXPRESSION
                                       "% Given clauses 12" "= end of proof ="
    )
    add_test(
        NAME interpformat_test1
        COMMAND
            bash -c
            "${CMAKE_INSTALL_PREFIX}/bin/mace4 -v0 -f ${CMAKE_CURRENT_LIST_DIR}/mace4.examples/group2.in | ${CMAKE_INSTALL_PREFIX}/bin/interpformat tabular"
    )
    set_property(
        TEST interpformat_test1 PROPERTY PASS_REGULAR_EXPRESSION "% number = 1"
                                         "% Interpretation of size 6"
    )
    add_test(
        NAME isofilter_test1
        COMMAND
            bash -c
            "${CMAKE_INSTALL_PREFIX}/bin/mace4 -n3 -m -1 -f ${CMAKE_CURRENT_LIST_DIR}/apps.examples/qg.in | ${CMAKE_INSTALL_PREFIX}/bin/interpformat | ${CMAKE_INSTALL_PREFIX}/bin/isofilter"
    )
    set_property(
        TEST isofilter_test1 PROPERTY PASS_REGULAR_EXPRESSION "% isofilter:"
                                      "kept=5"
    )
endif()

if(TEST_CPP)
    message("Building test_cpp")
    
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_options(-g)
    
    # Create a library for mace4 and its dependencies
    # add_library(mace4_lib STATIC
    #     ladr/accanon.c
    #     ladr/attrib.c
    #     ladr/avltree.c
    #     ladr/banner.c
    #     ladr/basic.c
    #     ladr/btm.c
    #     ladr/btu.c
    #     ladr/clash.c
    #     ladr/clause_misc.c
    #     ladr/clauses.c
    #     ladr/clauseid.c
    #     ladr/clausify.c
    #     ladr/clist.c
    #     ladr/cnf.c
    #     ladr/clock.c
    #     ladr/complex.c
    #     ladr/compress.c
    #     ladr/definitions.c
    #     ladr/demod.c
    #     ladr/dioph.c
    #     ladr/discrim.c
    #     ladr/discrimb.c
    #     ladr/discrimw.c
    #     ladr/dollar.c
    #     ladr/fastparse.c
    #     ladr/fatal.c
    #     ladr/flatdemod.c
    #     ladr/flatterm.c
    #     ladr/formula.c
    #     ladr/fpa.c
    #     ladr/fpalist.c
    #     ladr/glist.c
    #     ladr/hash.c
    #     ladr/interp.c
    #     ladr/int_code.c
    #     ladr/ioutil.c
    #     ladr/ivy.c
    #     ladr/just.c
    #     ladr/lindex.c
    #     ladr/listterm.c
    #     ladr/literals.c
    #     ladr/maximal.c
    #     ladr/memory.c
    #     ladr/mindex.c
    #     ladr/multiset.c
    #     ladr/nonport.c
    #     ladr/options.c
    #     ladr/order.c
    #     ladr/paramod.c
    #     ladr/parautil.c
    #     ladr/parse.c
    #     ladr/random.c
    #     ladr/resolve.c
    #     ladr/string.c
    #     ladr/strbuf.c
    #     ladr/subsume.c
    #     ladr/symbols.c
    #     ladr/term.c
    #     ladr/termflag.c
    #     ladr/termorder.c
    #     ladr/tlist.c
    #     ladr/topform.c
    #     ladr/unify.c
    #     ladr/weight.c
    #     ladr/xproofs.c
    #     mace4.src/arithmetic.c
    #     mace4.src/estack.c
    #     mace4.src/ground.c
    #     mace4.src/msearch.c
    #     mace4.src/mstate.c
    #     mace4.src/negprop.c
    #     mace4.src/negpropindex.c
    #     mace4.src/ordercells.c
    #     mace4.src/print.c
    #     mace4.src/propagate.c
    #     mace4.src/select.c
    #     mace4.src/syms.c
    #     mace4.src/util.c
    # )
    # target_link_libraries(mace4_lib PRIVATE test_cpp)
    # add_subdirectory("ladr")
    #add_subdirectory("mace4.src")
    add_executable(test_cpp test_cpp.cpp)
    install(TARGETS test_cpp DESTINATION bin)
endif()


# Add code to copy MSYS-2.0.dll
if(MSYS)
    # Find MSYS2 installation directory
    find_path(MSYS2_ROOT_DIR
        NAMES msys-2.0.dll
        PATHS "C:/msys64" "C:/msys32"
        DOC "MSYS2 installation directory"
    )
    
    if(MSYS2_ROOT_DIR)
        # Copy DLL to build directory
        # Run during configure time
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy
                "${MSYS2_ROOT_DIR}/msys-2.0.dll"
                "${CMAKE_BINARY_DIR}/msys-2.0.dll"
        )
        
        # Copy DLL to install directory
        install(FILES "${MSYS2_ROOT_DIR}/msys-2.0.dll"
            DESTINATION bin
        )
    else()
        message(WARNING "MSYS2 installation not found. DLL copying will be skipped.")
    endif()
endif()

