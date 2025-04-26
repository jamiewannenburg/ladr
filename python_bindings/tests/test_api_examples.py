import pytest
from ladr import Term, variables, constants, unary, binary, nary

def test_usage_example():
    """
    Example of how to use the ladr Term API.
    This test demonstrates the basic usage of the Term API.
    """
    x, y = variables('x y')
    a, b = constants('a b')
    f = binary('f')
    g = nary('g', 3)
    
    # Basic term creation
    t1 = f(x, y)
    assert t1.symbol == 'f'    
    assert t1.arity == 2
    assert t1.args[0] == x
    assert t1.args[1] == y
    
    # Term with multiple arguments
    t2 = g(x, y, a)
    assert t2.symbol == 'g'
    assert t2.arity == 3
    assert t2.args[0] == x
    assert t2.args[1] == y
    assert t2.args[2] == a
    
    # Nested terms
    t3 = f(t1, t2)
    assert t3.symbol == 'f'
    assert t3.arity == 2
    assert t3.args[0] == t1
    assert t3.args[1] == t2
    
# Add your new API tests below
# You can organize them as separate test functions

def test_my_new_api():
    """
    Example of a test for a new API feature.
    Replace with your actual test.
    """
    # Your test code here
    pass 