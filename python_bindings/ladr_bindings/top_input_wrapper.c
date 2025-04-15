#include "top_input_wrapper.h"
#include "../../ladr/top_input.h"


Plist embed_formulas_in_topforms_wrapper(Plist formulas, int is_input) {
    // convert is_input to BOOL
    BOOL is_input_bool = is_input ? TRUE : FALSE;
    // call embed_formulas_in_topforms
    Plist result = embed_formulas_in_topforms(formulas, is_input_bool);
    return result;
}