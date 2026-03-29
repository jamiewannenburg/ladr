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

cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(ladr)

if(MSVC)
  add_compile_options($<$<COMPILE_LANGUAGE:C>:/experimental:c11atomics>)
endif()

option(LADR "Build the LADR static library" ON)
option(MACE4 "Build the mace4 binaries" ON)
option(PROVER9 "Build the prover9 binaries" ON)
option(APPS "Build the LADR applications binaries" ON)

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

    # Pipelines: bash on Unix. On Windows use generated .cmd scripts run via cmake -E chdir ... cmd /c:
    #   (1) CTest invokes cmd.exe with "/c" quoted; cmd then errors ("The syntax of the command is incorrect").
    #   (2) PowerShell between native EXEs can buffer/encode stdout; mace4 prints a lot before models, so
    #       interpformat may see no interpretations on CI (regex miss) while prover9|prooftrans still passes.
    # Batch lines use real OS pipes. $<CONFIG> matches ctest -C (e.g. Test in CI, Release locally).
    # $<SHELL_PATH:...> forces native backslashes (CMake 3.19+).
    set(_x2 "${CMAKE_CURRENT_LIST_DIR}/prover9.examples/x2.in")
    set(_g2 "${CMAKE_CURRENT_LIST_DIR}/mace4.examples/group2.in")
    set(_qg "${CMAKE_CURRENT_LIST_DIR}/apps.examples/qg.in")
    if(WIN32)
        file(
            GENERATE
            OUTPUT "${CMAKE_BINARY_DIR}/ladr_ctest_prooftrans_$<CONFIG>.cmd"
            CONTENT
                "@echo off\r\n\"$<SHELL_PATH:$<TARGET_FILE:prover9>>\" -f \"$<SHELL_PATH:${_x2}>\" | \"$<SHELL_PATH:$<TARGET_FILE:prooftrans>>\" parents_only\r\n"
        )
        file(
            GENERATE
            OUTPUT "${CMAKE_BINARY_DIR}/ladr_ctest_interpformat_$<CONFIG>.cmd"
            CONTENT
                "@echo off\r\n\"$<SHELL_PATH:$<TARGET_FILE:mace4_exe>>\" -v0 -f \"$<SHELL_PATH:${_g2}>\" | \"$<SHELL_PATH:$<TARGET_FILE:interpformat>>\" tabular\r\n"
        )
        file(
            GENERATE
            OUTPUT "${CMAKE_BINARY_DIR}/ladr_ctest_isofilter_$<CONFIG>.cmd"
            CONTENT
                "@echo off\r\n\"$<SHELL_PATH:$<TARGET_FILE:mace4_exe>>\" -n3 -m -1 -f \"$<SHELL_PATH:${_qg}>\" | \"$<SHELL_PATH:$<TARGET_FILE:interpformat>>\" | \"$<SHELL_PATH:$<TARGET_FILE:isofilter>>\"\r\n"
        )
        add_test(
            NAME prooftrans_test1
            COMMAND
                "${CMAKE_COMMAND}" -E chdir "${CMAKE_BINARY_DIR}" cmd /c
                "ladr_ctest_prooftrans_$<CONFIG>.cmd"
        )
        add_test(
            NAME interpformat_test1
            COMMAND
                "${CMAKE_COMMAND}" -E chdir "${CMAKE_BINARY_DIR}" cmd /c
                "ladr_ctest_interpformat_$<CONFIG>.cmd"
        )
        add_test(
            NAME isofilter_test1
            COMMAND
                "${CMAKE_COMMAND}" -E chdir "${CMAKE_BINARY_DIR}" cmd /c
                "ladr_ctest_isofilter_$<CONFIG>.cmd"
        )
    else()
        add_test(
            NAME prooftrans_test1
            COMMAND
                bash -c
                "$<TARGET_FILE:prover9> -f ${_x2} | $<TARGET_FILE:prooftrans> parents_only"
        )
        add_test(
            NAME interpformat_test1
            COMMAND
                bash -c
                "$<TARGET_FILE:mace4_exe> -v0 -f ${_g2} | $<TARGET_FILE:interpformat> tabular"
        )
        add_test(
            NAME isofilter_test1
            COMMAND
                bash -c
                "$<TARGET_FILE:mace4_exe> -n3 -m -1 -f ${_qg} | $<TARGET_FILE:interpformat> | $<TARGET_FILE:isofilter>"
        )
    endif()
    set_property(
        TEST prooftrans_test1 PROPERTY PASS_REGULAR_EXPRESSION
                                       "% Given clauses 12" "= end of proof ="
    )
    set_property(
        TEST interpformat_test1 PROPERTY PASS_REGULAR_EXPRESSION "% number = 1"
                                         "% Interpretation of size 6"
    )
    set_property(
        TEST isofilter_test1 PROPERTY PASS_REGULAR_EXPRESSION "% isofilter:"
                                      "kept=5"
    )
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

