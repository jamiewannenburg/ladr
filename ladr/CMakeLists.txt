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

add_library(
    ladr STATIC
    ac_redun.c
    accanon.c
    attrib.c
    avltree.c
    backdemod.c
    banner.c
    basic.c
    btm.c
    btu.c
    clash.c
    clause_eval.c
    clause_misc.c
    clauseid.c
    clauses.c
    clausify.c
    clist.c
    clock.c
    cnf.c
    complex.c
    compress.c
    definitions.c
    demod.c
    di_tree.c
    dioph.c
    discrim.c
    discrimb.c
    discrimw.c
    dollar.c
    fastparse.c
    fatal.c
    features.c
    flatdemod.c
    flatterm.c
    formula.c
    fpa.c
    fpalist.c
    glist.c
    hash.c
    hints.c
    ibuffer.c
    int_code.c
    interp.c
    ioutil.c
    ivy.c
    just.c
    lindex.c
    listterm.c
    literals.c
    maximal.c
    memory.c
    mindex.c
    multiset.c
    nonport.c
    options.c
    order.c
    paramod.c
    parautil.c
    parse.c
    pindex.c
    random.c
    resolve.c
    std_options.c
    strbuf.c
    string.c
    subsume.c
    symbols.c
    term.c
    termflag.c
    termorder.c
    test.c
    tlist.c
    top_input.c
    topform.c
    tptp_trans.c
    unify.c
    weight.c
    weight2.c
    xproofs.c
)
