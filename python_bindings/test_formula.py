#!/usr/bin/env python3
"""
Test script for the LADR formula bindings
"""

import sys
import os

# Add the build directory to the Python path
sys.path.append(os.path.join(os.path.dirname(__file__), 'build'))

import formula

def test_basic_formulas():
    print("Testing basic formula operations:")
    
    # Create TRUE and FALSE formulas
    t = formula.get_true_formula()
    f = formula.get_false_formula()
    
    print(f"TRUE formula: {t}")
    print(f"Is it TRUE? {formula.true_formula(t)}")
    
    print(f"FALSE formula: {f}")
    print(f"Is it FALSE? {formula.false_formula(f)}")
    
    # Create a negation
    not_t = formula.negate(t)
    print(f"NOT TRUE: {not_t}")
    print(f"Is it FALSE? {formula.false_formula(not_t)}")
    
    # Create a conjunction and disjunction
    and_formula = formula.and_form(t, t)
    print(f"TRUE AND TRUE: {and_formula}")
    print(f"Is it TRUE? {formula.true_formula(and_formula)}")
    
    or_formula = formula.or_form(t, f)
    print(f"TRUE OR FALSE: {or_formula}")
    print(f"Is it TRUE? {formula.true_formula(or_formula)}")
    
    # Create implications
    imp = formula.imp_form(t, f)
    print(f"TRUE IMPLIES FALSE: {imp}")
    print(f"Is it FALSE? {formula.false_formula(imp)}")
    
    impby = formula.impby_form(f, t)
    print(f"FALSE IMPLIED BY TRUE: {impby}")
    print(f"Is it FALSE? {formula.false_formula(impby)}")

def test_formula_properties():
    print("\nTesting formula properties:")
    
    # Create a TRUE formula and check its properties
    t = formula.get_true_formula()
    print(f"TRUE formula type: {t.type}")
    print(f"TRUE formula arity: {t.arity}")
    print(f"TRUE formula is atom? {t.is_atom}")
    print(f"TRUE formula is quantified? {t.is_quantified}")

def main():
    print("LADR Formula Bindings Test")
    print("=========================")
    
    test_basic_formulas()
    test_formula_properties()
    
    print("\nAll tests completed successfully!")

if __name__ == "__main__":
    main() 