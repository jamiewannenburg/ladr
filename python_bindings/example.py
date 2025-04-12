#!/usr/bin/env python3
"""
Example script demonstrating the use of the LADR Python bindings.
This example shows how to use term, formula, and parse modules together.
"""

from ladr import term
from ladr import formula
from ladr import parse
from ladr_bindings.parse_wrapper import Parser

def example_term_creation():
    """Example of creating and manipulating terms."""
    print("\n=== Term Creation ===")
    
    # Create a variable term
    v0 = term.get_variable_term(0)
    print(f"Variable: {v0}")
    
    # Create a constant term
    a = term.term0("a")
    print(f"Constant: {a}")
    
    # Create a unary term
    f_a = term.term1("f", a)
    print(f"Unary term: {f_a}")
    
    # Create a binary term
    g_a_v0 = term.term2("g", a, v0)
    print(f"Binary term: {g_a_v0}")
    
    # Check if a term is ground (no variables)
    print(f"Is 'a' ground? {term.ground_term(a)}")
    print(f"Is 'g(a, v0)' ground? {term.ground_term(g_a_v0)}")

def example_parsing():
    """Example of parsing terms using the parse module."""
    print("\n=== Parsing ===")
    
    # Initialize a parser with standard parse types
    parser = Parser()
    
    # Parse a simple term
    t1 = parser.parse("f(x, g(y, z))")
    print(f"Parsed term: {t1}")
    print(f"Term as string: {parser.to_string(t1)}")
    
    # Declare a custom operator
    parser.declare_operator("&", 500, parse.ParseType.INFIX_LEFT)
    print("Declared '&' as an infix operator with precedence 500")
    
    # Testing parenthesization
    parser.set_parenthesize(False)
    t2 = parser.parse("f(a, b)")
    print(f"Without parenthesis: {parser.to_string(t2)}")
    
    parser.set_parenthesize(True)
    print(f"With parenthesis: {parser.to_string(t2)}")

def example_formula():
    """Example of creating and manipulating formulas."""
    print("\n=== Formula Creation ===")
    
    # Create formulas from terms
    eq_term = term.term2("=", term.term0("a"), term.term0("b"))
    eq_formula = formula.term_to_formula(eq_term)
    print(f"Equality formula: {eq_formula}")
    
    # Create negation of the equality
    not_eq = formula.negate(eq_formula)
    print(f"Negated formula: {not_eq}")
    
    # Create TRUE and FALSE formulas
    true_f = formula.get_true_formula()
    false_f = formula.get_false_formula()
    print(f"True formula: {true_f}")
    print(f"False formula: {false_f}")
    
    # Create logical connectives
    p_term = term.term0("P")
    q_term = term.term0("Q")
    p_formula = formula.term_to_formula(p_term)
    q_formula = formula.term_to_formula(q_term)
    
    # AND formula
    and_formula = formula.and_form(p_formula, q_formula)
    print(f"AND formula: {and_formula}")
    
    # OR formula
    or_formula = formula.or_form(p_formula, q_formula)
    print(f"OR formula: {or_formula}")
    
    # IMPLICATION formula
    imp_formula = formula.imp_form(p_formula, q_formula)
    print(f"IMPLICATION formula: {imp_formula}")

def main():
    """Main function demonstrating the LADR Python bindings."""
    print("LADR Python Bindings Example")
    
    example_term_creation()
    example_parsing()
    example_formula()
    
    print("\nExample complete!")

if __name__ == "__main__":
    main() 