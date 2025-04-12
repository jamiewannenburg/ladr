#ifndef FORMULA_WRAPPER_H
#define FORMULA_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../ladr/header.h"
#include "../../ladr/formula.h"
#include "../../ladr/term.h"

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
// C++ wrapper functions for reserved keywords
inline Formula formula_and(Formula a, Formula b) {
    extern Formula and(Formula a, Formula b);
    return and(a, b);
}

inline Formula formula_or(Formula a, Formula b) {
    extern Formula or(Formula a, Formula b);
    return or(a, b);
}

// Wrapper for operator parameter in attribute functions if needed
inline Term attributes_to_term_wrapper(Attribute a, char *op_str) {
    extern Term attributes_to_term(Attribute a, char *operator_param);
    return attributes_to_term(a, op_str);
}

inline Attribute term_to_attributes_wrapper(Term t, char *op_str) {
    extern Attribute term_to_attributes(Term t, char *operator_param);
    return term_to_attributes(t, op_str);
}
#endif

#endif // FORMULA_WRAPPER_H 