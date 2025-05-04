import pytest
import sys
from ladr import (
    Term, variables, constants, unary, binary, nary, parse_term, 
    set_memory_limit, init_memory, top_input
)
import io
import tempfile
import os

# Initialize LADR for tests
#init_memory()
#set_memory_limit(0)  # Set to unlimited

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
    
def test_parse_term():
    """
    Should parse a real prover9 string and return a Term object.
    """
    # Try simple parsing first to verify it works
    simple_term = parse_term("x^y")
    assert simple_term is not None
    print(f"Successfully parsed simple term: {simple_term}")
    
def test_read_from_file():
    # Now try the more complex example
    p9m4_file = r"""% Saved by Prover9-Mace4 Version 0.5, December 2007.

set(ignore_option_dependencies). % GUI handles dependencies

if(Prover9). % Options for Prover9
  assign(max_seconds, 120).
end_if.

if(Mace4).   % Options for Mace4
  assign(start_size, 8).
  assign(end_size, 8).
  assign(max_models, -1).
  assign(max_seconds, -1).
end_if.

formulas(assumptions).

%lattice
x^x=x.
x^y=y^x.
x^(y^z)=(x^y)^z.
x v x=x.
x v y=y v x.
x v (y v z)=(x v y) v z.
x ^ (x v y) = x.
x v (x ^ y) = x.

%distributive
x^(y v z) = (x^y)v(x^z).

%leq
(x<=y)<->(x v y=y).

%crl
x*y = y*x.
e*x=x.
x*(y*z)=(x*y)*z.
x<=y\(y*x).
x*(x\y) <= y.
x*(y^z)<= (x*y)^(x*z).
(x\y)^(x\z) = x\(y^z).

%involution1
x = ~(~(x)).
%x\y = ~(x*~(y)).
~(x) = x\(~(e)).

%square increasing
x<=x*x.

%KR
e <= (x^~(x))\y.

%simple
(x<=e)->(x=e | x=~(~(e)*~(e))).

end_of_list.

formulas(goals).

end_of_list.
"""
    in_file = tempfile.NamedTemporaryFile(delete=False)
    with open(in_file.name, 'wb') as f:
        f.write(p9m4_file.encode('ascii'))
    try:
        top_input.read_from_file(in_file.name, echo=True, unknown_action=0)
    except Exception as e:
        print(f"Error parsing full file: {e}")
        raise
    in_file.close()
    os.remove(in_file.name)

if __name__ == '__main__':
    test_parse_term()
    test_read_from_file()