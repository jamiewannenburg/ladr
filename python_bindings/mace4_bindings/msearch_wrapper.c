#include "msearch_wrapper.h"
#include "../../mace4.src/msearch.h"
#include "../../ladr/glist.h"
#include <Python.h>

void msearch_init_mace_options(Mace_options opt) {
    init_mace_options(opt);
}

Mace_results msearch_mace4(Plist clauses, Mace_options opt) {
    return mace4(clauses, opt);
}

void msearch_mace4_exit(int exit_code) {
    mace4_exit(exit_code);
}
