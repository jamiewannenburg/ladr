"""
Test different syntax formats for LADR term parsing.
"""
from ladr import parse_term, LadrFatalError, init_parser #, set_memory_limit, reset_error_flags

# Set unlimited memory
#set_memory_limit(0)

# Ensure parser is initialized
# TODO: Add package options for this
#init_parser()

def try_parse(syntax, description=""):
    """Try parsing a term with the given syntax and report success or failure."""
    print(f"\nTrying to parse: {syntax} {description}")
    #reset_error_flags()
    try:
        term = parse_term(syntax)
        print(f"SUCCESS: Parsed as {term}")
        return term
    except LadrFatalError as e:
        print(f"FAILED: {e}")
        return None

# Try different formats
print("Testing LADR term syntax variants...")

# Try with commas (common in many systems, but not LADR)
try_parse("f(x, y)", "(comma-separated arguments)")

# Try with regular terms
try_parse("f(x)", "(single argument)")
try_parse("f(g(x))", "(nested function)")
try_parse("f(g(x),h(y))", "(multiple complex args)")

# Try with operators
try_parse("x + y", "(infix operator)")
try_parse("x+y", "(infix operator without spaces)")
try_parse("x^y", "(power/conjunction operator)")
try_parse("x v y", "(disjunction operator)")

# Try constants only
try_parse("a", "(constant)")
try_parse("f", "(function symbol alone)")

# Try with variables
try_parse("X", "(capitalized variable)")
try_parse("x", "(lowercase, could be constant or variable)")

# Try defining formulas
try_parse("x = y", "(equation)")
try_parse("P(x) & Q(y)", "(logical AND)")
try_parse("P(x) | Q(y)", "(logical OR)")
try_parse("P(x) -> Q(y)", "(implication)")
try_parse("P(x) <-> Q(y)", "(equivalence)")
try_parse("all x P(x)", "(universal quantifier)")
try_parse("exists x P(x)", "(existential quantifier)")


if __name__ == "__main__":
    print("\nDone testing syntax variants.") 