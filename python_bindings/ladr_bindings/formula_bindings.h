/*
 * Special header for pybind11 to isolate C++ keyword conflicts
 * This file provides declarations for the functions and types from formula.h
 * but avoids the use of C++ keywords like 'and', 'or', and 'operator'.
 */

#ifndef FORMULA_BINDINGS_H
#define FORMULA_BINDINGS_H

#include <stdio.h>

// Include main header for BOOL
extern "C" {
#include "../../ladr/header.h"
}

// Forward declarations
typedef struct formula *Formula;
typedef struct term *Term;
typedef struct attribute *Attribute;

// formula types
typedef enum {
  ATOM_FORM=0, AND_FORM, OR_FORM, NOT_FORM, IFF_FORM,
  IMP_FORM, IMPBY_FORM, ALL_FORM, EXISTS_FORM
} Ftype;

// These are macros in the original code
inline int TRUE_FORMULA(Formula f) {
    return f->type == AND_FORM && f->arity == 0;
}

inline int FALSE_FORMULA(Formula f) {
    return f->type == OR_FORM && f->arity == 0;
}

// Exported C functions we need with problematic names fixed
extern "C" {
    // Memory management
    void free_formula(Formula p);
    Formula formula_get(int arity, Ftype type);
    void zap_formula(Formula f);
    
    // Formula accessors
    int formula_size(Formula f);
    BOOL quant_form(Formula f);
    BOOL literal_formula(Formula f);
    BOOL clausal_formula(Formula f);
    BOOL closed_formula(Formula f);
    BOOL formula_ident(Formula f, Formula g);
    Formula formula_copy(Formula f);
    
    // Term functions
    Term copy_term(Term t);
    BOOL ground_term(Term t);
    
    // Term/formula conversions
    Term formula_to_term(Formula f);
    Formula term_to_formula(Term t);
    
    // Output
    void fprint_formula(FILE *fp, Formula f);
    
    // Formula operations - renamed the problematic ones
    Formula negate(Formula a);
    Formula imp(Formula a, Formula b);
    Formula impby(Formula a, Formula b);
    Formula flatten_top(Formula f);
    Formula formula_flatten(Formula f);
    Formula nnf(Formula f);
    Formula universal_closure(Formula f);
    
    // These names are C++ keywords, so we need wrapper functions
    Formula formula_and(Formula a, Formula b);  // renamed from 'and'
    Formula formula_or(Formula a, Formula b);   // renamed from 'or'
}

// Now define our renamed functions (implemented in formula.c)
extern "C" {
    // Define formula_and in terms of the C function 'and'
    Formula formula_and(Formula a, Formula b);
    Formula formula_or(Formula a, Formula b);
}

#endif // FORMULA_BINDINGS_H 