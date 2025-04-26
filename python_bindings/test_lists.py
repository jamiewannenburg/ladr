"""
Test script for the Plist and Ilist conversions.
"""

from ladr.term import Term, get_variable_term, get_rigid_term, term2, set_of_variables, copy_term

def test_term_variables():
    """Test set_of_variables to get variables from a term"""
    try:
        # Create a term: f(x, g(y, x))
        x = get_variable_term(0)  # Variable x (index 0)
        y = get_variable_term(1)  # Variable y (index 1)
        
        # Create the term using term2
        f_xy = term2("f", x, y)
        
        # Get the variables in the term
        variables = set_of_variables(f_xy)
        
        # Should print a list with two variable terms (x and y)
        print(f"Term: {f_xy}")
        print(f"Variables in term ({len(variables)}): {[str(v) for v in variables]}")
        
        # Create a more complex term
        g_yx = term2("g", y, x)
        f_complex = term2("f", f_xy, g_yx)
        
        # Get variables from term
        variables2 = set_of_variables(f_complex)
        print(f"Complex term: {f_complex}")
        print(f"Variables in complex term ({len(variables2)}): {[str(v) for v in variables2]}")
        
        print("Term variables test passed!")
        return True
    except Exception as e:
        print(f"Term variables test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    print("\nTesting term variables extraction:")
    vars_ok = test_term_variables()
    
    if vars_ok:
        print("\nAll tests passed!")
    else:
        print("\nSome tests failed!") 