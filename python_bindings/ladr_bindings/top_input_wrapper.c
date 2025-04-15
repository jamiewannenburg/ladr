#include "top_input_wrapper.h"
#include "../../ladr/top_input.h"
#include "glist_wrapper.h"



void *embed_formulas_in_topforms_wrapper(void *formulas, int is_input) {
    // convert python list to Plist
    Plist plist = python_list_to_plist(formulas);
    // convert is_input to BOOL
    BOOL is_input_bool = is_input ? TRUE : FALSE;
    // call embed_formulas_in_topforms
    Plist result = embed_formulas_in_topforms(plist, is_input_bool);
    // convert Plist to python list
    return plist_to_python_list(result); //TODO needed? does pybind not do this?
}