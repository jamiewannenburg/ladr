import unittest
from ladr import Term, variables, constants, unary, binary, nary
import debug

class TestSympyTerm(unittest.TestCase):

        
    #@debug.traced
    def test_variable_creation(self):
        x = variables('x')
        self.assertIsInstance(x, Term)
        self.assertTrue(x.is_variable)
        self.assertFalse(x.is_constant)
        self.assertFalse(x.is_complex)
        self.assertEqual(x.symbol, 'x')
        self.assertEqual(x.arity, 0)
        self.assertEqual(len(x.args), 0)

        y, z = variables('y z')
        self.assertIsInstance(y, Term)
        self.assertTrue(y.is_variable)
        self.assertEqual(y.symbol, 'y')
        self.assertIsInstance(z, Term)
        self.assertTrue(z.is_variable)
        self.assertEqual(z.symbol, 'z')

    #@debug.traced
    def test_constant_creation(self):
        a = constants('a')
        self.assertIsInstance(a, Term)
        self.assertFalse(a.is_variable)
        self.assertTrue(a.is_constant)
        self.assertFalse(a.is_complex)
        self.assertEqual(a.symbol, 'a')
        self.assertEqual(a.arity, 0)
        self.assertEqual(len(a.args), 0)
        self.assertEqual(str(a), 'a')

        b, C = constants('b Constant') # Multi-char constant
        self.assertIsInstance(b, Term)
        self.assertTrue(b.is_constant)
        self.assertEqual(b.symbol, 'b')
        self.assertIsInstance(C, Term)
        self.assertTrue(C.is_constant)
        self.assertEqual(C.symbol, 'Constant')

        # Test caching / identity
        a2 = constants('a')
        self.assertEqual(a, a2)

    #@debug.traced
    def test_operator_overloading(self):
        x, y = variables('x y')
        a, b = constants('a b')

        # Addition
        add_xy = x + y
        self.assertTrue(add_xy.is_complex)
        self.assertEqual(add_xy.symbol, '+')
        self.assertEqual(add_xy.arity, 2)
        self.assertEqual(add_xy.args[0], x)
        self.assertEqual(add_xy.args[1], y)
        # self.assertEqual(str(add_xy), 'x+y')

        # Subtraction
        sub_ay = a - y
        self.assertTrue(sub_ay.is_complex)
        self.assertEqual(sub_ay.symbol, '-')
        self.assertEqual(sub_ay.arity, 2)
        self.assertEqual(sub_ay.args[0], a)
        self.assertEqual(sub_ay.args[1], y)
        # self.assertEqual(str(sub_ay), 'a-y')

        # Multiplication
        mul_xa = x * a
        self.assertTrue(mul_xa.is_complex)
        self.assertEqual(mul_xa.symbol, '*')
        self.assertEqual(mul_xa.arity, 2)
        self.assertEqual(mul_xa.args[0], x)
        self.assertEqual(mul_xa.args[1], a)
        # self.assertEqual(str(mul_xa), 'x*a')

        # Division
        div_ab = a / b
        self.assertTrue(div_ab.is_complex)
        self.assertEqual(div_ab.symbol, '/')
        self.assertEqual(div_ab.arity, 2)
        self.assertEqual(div_ab.args[0], a)
        self.assertEqual(div_ab.args[1], b)
        # self.assertEqual(str(div_ab), 'a/b')

        # Negation
        neg_y = -y
        self.assertTrue(neg_y.is_complex)
        self.assertEqual(neg_y.symbol, '-')
        self.assertEqual(neg_y.arity, 1)
        self.assertEqual(neg_y.args[0], y)
        # self.assertEqual(str(neg_y), '-y')

        # --- Added Bitwise Operators ---
        # Bitwise AND
        and_xy = x & y
        self.assertTrue(and_xy.is_complex)
        self.assertEqual(and_xy.symbol, '&')
        self.assertEqual(and_xy.arity, 2)
        self.assertEqual(and_xy.args[0], x)
        self.assertEqual(and_xy.args[1], y)
        # self.assertEqual(str(and_xy), 'x&y')

        # Bitwise OR
        or_xa = x | a
        self.assertTrue(or_xa.is_complex)
        self.assertEqual(or_xa.symbol, '|')
        self.assertEqual(or_xa.arity, 2)
        self.assertEqual(or_xa.args[0], x)
        self.assertEqual(or_xa.args[1], a)
        # self.assertEqual(str(or_xa), 'x|a')

        # Bitwise XOR
        xor_ab = a ^ b
        self.assertTrue(xor_ab.is_complex)
        self.assertEqual(xor_ab.symbol, '^')
        self.assertEqual(xor_ab.arity, 2)
        self.assertEqual(xor_ab.args[0], a)
        self.assertEqual(xor_ab.args[1], b)
        # self.assertEqual(str(xor_ab), 'a^b')

        # # Mixed / Chaining (with new operators)
        # expr = (x + y) * a - (b / x) | (a & y)
        # # Expected structure: sub( mul( add(x,y), a ), bitor ( div( b, x ), bitand(a, y) ) )
        # # Simplified str: ((x+y)*a)-(b/x)|(a&y)
        # # Note: Parenthesization rules in __str__ might need refinement for mixed precedence.
        # # Current basic check:
        # self.assertEqual(expr.symbol, '-') # Subtraction is the outer operation
        # self.assertEqual(expr.arity, 2)
        # # Check left side of sub
        # mul_term = expr.args[0]
        # self.assertEqual(mul_term.symbol, '*')
        # self.assertEqual(mul_term.args[0].symbol, '+') # x + y
        # self.assertEqual(mul_term.args[1], a) # a
        # # Check right side of sub
        # bitor_term = expr.args[1]
        # self.assertEqual(bitor_term.symbol, 'bitor')
        # self.assertEqual(bitor_term.args[0].symbol, 'div') # b / x
        # self.assertEqual(bitor_term.args[1].symbol, 'bitand') # a & y

        # Check string representation - depends heavily on __str__ parenthesization logic
        # self.assertEqual(str(expr), '((x+y)*a)-((b/x)|(a&y))') # Example if fully parenthesized
        # Check repr representation
        # expected_repr = "(( (variables('x') + variables('y')) * constants('a')) - ( (constants('b') / variables('x')) | (constants('a') & variables('y')) ))"
        # self.assertEqual(repr(expr), expected_repr)
    
    #@debug.traced
    def test_usage_example(self):
        x, y = variables('x y')
        a, b = constants('a b')
        f = binary('f')
        g = nary('g',3)
        t1 = f(x,y)
        self.assertEqual(t1.symbol, 'f')    
        self.assertEqual(t1.arity, 2)
        self.assertEqual(t1.args[0], x)
        self.assertEqual(t1.args[1], y)
        # self.assertEqual(str(t1), 'f(x,y)')
        t2 = g(x,y,a)
        self.assertEqual(t2.symbol, 'g')
        self.assertEqual(t2.arity, 3)
        self.assertEqual(t2.args[0], x)
        self.assertEqual(t2.args[1], y)
        self.assertEqual(t2.args[2], a)
        # self.assertEqual(str(t2), 'g(x,y,a)')
        t3 = f(t1,t2)
        self.assertEqual(t3.symbol, 'f')
        self.assertEqual(t3.arity, 2)
        self.assertEqual(t3.args[0], t1)
        self.assertEqual(t3.args[1], t2)
        # self.assertEqual(str(t3), 'f(f(x,y),g(x,y,a))')
        
        
        
if __name__ == '__main__':
    #debug.reexecute_if_unbuffered()
    unittest.main() 