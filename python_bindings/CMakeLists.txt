cmake_minimum_required(VERSION 3.18)
project(ladr)

# Find Python and pybind11
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Find NumPy
execute_process(
  COMMAND "${Python_EXECUTABLE}" -c "import numpy; print(numpy.get_include())"
  OUTPUT_VARIABLE NumPy_INCLUDE_DIRS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set position independent code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g)

# Create a library from our term implementation and all its dependencies
add_library(term_lib STATIC
    ../ladr/term.c
    ../ladr/symbols.c
    ../ladr/memory.c
    ../ladr/strbuf.c
    ../ladr/string.c
    ../ladr/hash.c
    ../ladr/avltree.c
    ../ladr/glist.c
    ../ladr/order.c
    ../ladr/fatal.c
)

# Create a library for formula and its dependencies
add_library(formula_lib STATIC
    ../ladr/formula.c
    ../ladr/term.c
    ../ladr/symbols.c
    ../ladr/memory.c
    ../ladr/strbuf.c
    ../ladr/string.c
    ../ladr/hash.c
    ../ladr/avltree.c
    ../ladr/glist.c
    ../ladr/order.c
    ../ladr/fatal.c
    ../ladr/attrib.c
    ../ladr/tlist.c
    ../ladr/termorder.c
    ../ladr/flatterm.c
    ../ladr/unify.c
    ../ladr/termflag.c
    ../ladr/random.c
    ../ladr/int_code.c
    ../ladr/nonport.c
    ../ladr/parse.c
    ../ladr/multiset.c
    ../ladr/ioutil.c
    ../ladr/weight.c
    ../ladr/topform.c
    ../ladr/lindex.c
    ../ladr/mindex.c
    ../ladr/maximal.c
    ../ladr/subsume.c
    ../ladr/just.c
    ../ladr/resolve.c
    ../ladr/paramod.c
    ../ladr/parautil.c
    ../ladr/options.c
    ../ladr/listterm.c
    ladr_bindings/formula_wrapper.c
)

# Create a library for parse and its dependencies
add_library(parse_lib STATIC
    ../ladr/parse.c
    ../ladr/term.c
    ../ladr/symbols.c
    ../ladr/memory.c
    ../ladr/strbuf.c
    ../ladr/string.c
    ../ladr/hash.c
    ../ladr/avltree.c
    ../ladr/glist.c
    ../ladr/order.c
    ../ladr/fatal.c
    ../ladr/listterm.c
)

# Create a library for mace4 and its dependencies
add_library(mace4_lib STATIC
    ../ladr/accanon.c
    ../ladr/attrib.c
    ../ladr/avltree.c
    ../ladr/banner.c
    ../ladr/basic.c
    ../ladr/btm.c
    ../ladr/btu.c
    ../ladr/clash.c
    ../ladr/clause_misc.c
    ../ladr/clauses.c
    ../ladr/clauseid.c
    ../ladr/clausify.c
    ../ladr/clist.c
    ../ladr/cnf.c
    ../ladr/clock.c
    ../ladr/complex.c
    ../ladr/compress.c
    ../ladr/definitions.c
    ../ladr/demod.c
    ../ladr/dioph.c
    ../ladr/discrim.c
    ../ladr/discrimb.c
    ../ladr/discrimw.c
    ../ladr/dollar.c
    ../ladr/fastparse.c
    ../ladr/fatal.c
    ../ladr/flatdemod.c
    ../ladr/flatterm.c
    ../ladr/formula.c
    ../ladr/fpa.c
    ../ladr/fpalist.c
    ../ladr/glist.c
    ../ladr/hash.c
    ../ladr/interp.c
    ../ladr/int_code.c
    ../ladr/ioutil.c
    ../ladr/ivy.c
    ../ladr/just.c
    ../ladr/lindex.c
    ../ladr/listterm.c
    ../ladr/literals.c
    ../ladr/maximal.c
    ../ladr/memory.c
    ../ladr/mindex.c
    ../ladr/multiset.c
    ../ladr/nonport.c
    ../ladr/options.c
    ../ladr/order.c
    ../ladr/paramod.c
    ../ladr/parautil.c
    ../ladr/parse.c
    ../ladr/random.c
    ../ladr/resolve.c
    ../ladr/string.c
    ../ladr/strbuf.c
    ../ladr/subsume.c
    ../ladr/symbols.c
    ../ladr/term.c
    ../ladr/termflag.c
    ../ladr/termorder.c
    ../ladr/tlist.c
    ../ladr/topform.c
    ../ladr/unify.c
    ../ladr/weight.c
    ../ladr/xproofs.c
    ../mace4.src/arithmetic.c
    ../mace4.src/estack.c
    ../mace4.src/ground.c
    ../mace4.src/msearch.c
    ../mace4.src/mstate.c
    ../mace4.src/negprop.c
    ../mace4.src/negpropindex.c
    ../mace4.src/ordercells.c
    ../mace4.src/print.c
    ../mace4.src/propagate.c
    ../mace4.src/select.c
    ../mace4.src/syms.c
    ../mace4.src/util.c
    mace4_bindings/msearch_wrapper.c
)
target_include_directories(mace4_lib PRIVATE ${Python_INCLUDE_DIRS})

# Create the term module
pybind11_add_module(term ladr_bindings/term_bindings.cpp)
target_link_libraries(term PRIVATE term_lib)

# Create the formula module
pybind11_add_module(formula ladr_bindings/formula_bindings.cpp)
target_link_libraries(formula PRIVATE formula_lib)

# Create the parse module
pybind11_add_module(parse ladr_bindings/parse_bindings.cpp)
target_link_libraries(parse PRIVATE parse_lib)

# Create the mace4 module
pybind11_add_module(mace4 mace4_bindings/msearch_bindings.cpp)
target_link_libraries(mace4 PRIVATE mace4_lib Python::Python)
target_include_directories(mace4 PRIVATE ${Python_INCLUDE_DIRS})

# Install the Python modules
install(TARGETS term formula parse mace4 DESTINATION ladr)
