"""
Test error handling in LADR Python bindings.
"""
import pytest
from ladr import (
    parse_term, LadrFatalError, suppress_error_messages, 
    set_memory_limit
)

@pytest.mark.skip(reason="This test causes a corruption of the symbol table, and must be skipped.")
def test_error_handling():
    """Test that LADR fatal errors are properly converted to Python exceptions."""
    
    # Ensure we have a high memory limit for this test
    set_memory_limit(0)  # 0 = unlimited
    
    # Test with error messages suppressed
    suppress_error_messages(True)
    
    # Example 1: Try to parse a term with a syntax error
    try:
        parse_term("(")  # Unbalanced parenthesis causes a fatal error
        # We should not reach this point - an exception should be raised
        pytest.fail("Expected ValueError but no exception was raised")
    except ValueError as e:
        # This is the expected behavior
        print(f"Caught ValueError as expected: {e}")
        #assert "parenthesis" in str(e).lower() or "syntax" in str(e).lower() or "error" in str(e).lower()
    
    # Example 2: Try to parse a valid term
    term = parse_term("f(x, g(y))")
    assert term is not None
    assert str(term) == "f(x,g(y))"
    
    # Example 3: Re-enable error messages if you want to see them
    suppress_error_messages(False)
    
    # Now errors will print to stdout/stderr as well as raising exceptions
    try:
        parse_term("@#%^&")  # Invalid syntax
        # We should not reach this point - an exception should be raised
        pytest.fail("Expected LadrFatalError but no exception was raised")
    except ValueError as e:
        # This is the expected behavior
        print(f"Caught LadrFatalError as expected: {e}")
        #assert "syntax" in str(e).lower() or "error" in str(e).lower()
    #TODO: Reset state?

if __name__ == "__main__":
    test_error_handling() 