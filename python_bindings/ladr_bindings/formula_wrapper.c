/*
 * Wrapper functions for LADR formula operations
 * This file provides C wrappers for functions with names that conflict with C++ keywords
 */

#include <stdio.h>

// Avoid directly including formula.h to prevent conflicts
struct formula;
typedef struct formula *Formula;

// Forward declare the original functions
Formula and(Formula a, Formula b);
Formula or(Formula a, Formula b);

/* 
 * Wrapper for the 'and' function to avoid C++ keyword conflict
 */
Formula formula_and(Formula a, Formula b) {
    return and(a, b);
}

/* 
 * Wrapper for the 'or' function to avoid C++ keyword conflict
 */
Formula formula_or(Formula a, Formula b) {
    return or(a, b);
}
