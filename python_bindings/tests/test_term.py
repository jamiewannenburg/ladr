import unittest
from ladr import term
#import debug

class TestTerm(unittest.TestCase):
    #@debug.traced
    def test_constants(self):
        """Test the term module constants"""
        self.assertIsInstance(term.MAX_VARS, int)
        self.assertIsInstance(term.MAX_VNUM, int)
        self.assertIsInstance(term.MAX_VAR, int)
        self.assertIsInstance(term.MAX_SYM, int)
        self.assertIsInstance(term.MAX_ARITY, int)
    #@debug.traced
    def test_variable_term(self):
        """Test variable term creation and properties"""
        var = term.get_variable_term(1)
        self.assertTrue(var.is_variable)
        self.assertFalse(var.is_constant)
        self.assertFalse(var.is_complex)
        self.assertEqual(var.varnum, 1)
        self.assertEqual(var.arity, 0)
        self.assertEqual(str(var), "v1")
        self.assertEqual(len(var), 0)
        
        with self.assertRaises(ValueError):
            _ = var.symnum  # Variables don't have symnum
    #@debug.traced
    def test_constant_term(self):
        """Test constant term creation and properties"""
        const = term.term0("a")
        self.assertFalse(const.is_variable)
        self.assertTrue(const.is_constant)
        self.assertFalse(const.is_complex)
        self.assertEqual(const.symnum, abs(const.symnum))  # symnum is positive
        self.assertEqual(const.arity, 0)
        self.assertEqual(const.symbol, "a")
        self.assertEqual(str(const), "a")
        self.assertEqual(len(const), 0)
        
        with self.assertRaises(ValueError):
            _ = const.varnum  # Constants don't have varnum
    #@debug.traced
    def test_complex_term(self):
        """Test complex term creation and properties"""
        var1 = term.get_variable_term(1)
        var2 = term.get_variable_term(2)
        func = term.term2("f", var1, var2)
        
        self.assertFalse(func.is_variable)
        self.assertFalse(func.is_constant)
        self.assertTrue(func.is_complex)
        self.assertEqual(func.arity, 2)
        self.assertEqual(func.symbol, "f")
        self.assertEqual(str(func), "f(v1,v2)")
        self.assertEqual(len(func), 2)
        
        # Test argument access
        self.assertTrue(func[0].is_variable)
        self.assertEqual(func[0].varnum, 1)
        self.assertTrue(func[1].is_variable)
        self.assertEqual(func[1].varnum, 2)
        
        with self.assertRaises(IndexError):
            _ = func[2]  # Out of range
    #@debug.traced
    def test_term_operations(self):
        """Test term operations"""
        var = term.get_variable_term(1)
        const = term.term0("a")
        func = term.term1("f", var)
        
        # Test copy_term
        var_copy = term.copy_term(var)
        self.assertTrue(var_copy.is_variable)
        self.assertEqual(var_copy.varnum, 1)
        
        # Test term_ident
        self.assertTrue(term.term_ident(var, var_copy))
        self.assertFalse(term.term_ident(var, const))
        
        # Test is_term
        self.assertTrue(term.is_term(func, "f", 1))
        self.assertFalse(term.is_term(func, "g", 1))
        self.assertFalse(term.is_term(func, "f", 2))
        
        # Test is_constant
        self.assertTrue(term.is_constant(const, "a"))
        self.assertFalse(term.is_constant(const, "b"))
        self.assertFalse(term.is_constant(func, "f"))
        
        # Test ground_term
        self.assertTrue(term.ground_term(const))
        self.assertFalse(term.ground_term(var))
        self.assertFalse(term.ground_term(func))
        
        # Test term_depth
        self.assertEqual(term.term_depth(const), 0)
        self.assertEqual(term.term_depth(var), 0)
        self.assertEqual(term.term_depth(func), 1)
        
        # Test symbol_count
        self.assertEqual(term.symbol_count(const), 1)
        self.assertEqual(term.symbol_count(var), 1)
        self.assertEqual(term.symbol_count(func), 2)
        
        # Test occurs_in
        self.assertTrue(term.occurs_in(var, func))
        self.assertFalse(term.occurs_in(const, func))
    #@debug.traced
    def test_term_builders(self):
        """Test term builders"""
        var = term.get_variable_term(1)
        
        # Test term0, term1, term2
        const = term.term0("a")
        self.assertTrue(const.is_constant)
        self.assertEqual(const.symbol, "a")
        
        unary = term.term1("f", var)
        self.assertTrue(unary.is_complex)
        self.assertEqual(unary.symbol, "f")
        self.assertEqual(unary.arity, 1)
        
        binary = term.term2("g", var, const)
        self.assertTrue(binary.is_complex)
        self.assertEqual(binary.symbol, "g")
        self.assertEqual(binary.arity, 2)
        
        # Test build_unary_term and build_binary_term
        unary2 = term.build_unary_term("h", var)
        self.assertTrue(unary2.is_complex)
        self.assertEqual(unary2.symbol, "h")
        self.assertEqual(unary2.arity, 1)
        
        binary2 = term.build_binary_term("k", var, const)
        self.assertTrue(binary2.is_complex)
        self.assertEqual(binary2.symbol, "k")
        self.assertEqual(binary2.arity, 2)
    #@debug.traced
    def test_conversions(self):
        """Test conversion functions"""
        # Integer conversions
        int_term = term.int_to_term(42)
        self.assertTrue(int_term.is_constant)
        self.assertEqual(term.term_to_int(int_term), 42)
        
        # Natural number conversions
        nat_term = term.nat_to_term(7)
        self.assertTrue(nat_term.is_constant)
        self.assertEqual(term.term_to_int(nat_term), 7)
        
        # Boolean conversions - still use the BOOL enum for creating terms
        true_term = term.bool_to_term(term.BOOL.TRUE)
        self.assertTrue(true_term.is_constant)
        self.assertTrue(term.term_to_bool(true_term))
        
        false_term = term.bool_to_term(term.BOOL.FALSE)
        self.assertTrue(false_term.is_constant)
        self.assertFalse(term.term_to_bool(false_term))
        
        # Python boolean conversions
        py_true_term = term.bool_to_term(True)
        self.assertTrue(py_true_term.is_constant)
        self.assertTrue(term.term_to_bool(py_true_term))
        
        py_false_term = term.bool_to_term(False)
        self.assertTrue(py_false_term.is_constant)
        self.assertFalse(term.term_to_bool(py_false_term))
        
        # Test conversion failures
        var = term.get_variable_term(1)
        with self.assertRaises(ValueError):
            term.term_to_int(var)
        
        with self.assertRaises(ValueError):
            term.term_to_bool(var)

    def test_get_variable_term(self):
        """Test get_variable_term function"""
        # Create a term: f(x, g(y, x))
        x = term.get_variable_term(0)  # Variable x (index 0)
        y = term.get_variable_term(1)  # Variable y (index 1)
        
        # Create the term using term2
        f_xy = term.term2("f", x, y)
        
        # Get the variables in the term
        variables = term.set_of_variables(f_xy)
        self.assertEqual(len(variables), 2)
        self.assertIn(x, variables)
        self.assertIn(y, variables)
        
        # Create a more complex term
        g_yx = term.term2("g", y, x)
        f_complex = term.term2("f", f_xy, g_yx)
        
        # Get variables from term
        variables2 = term.set_of_variables(f_complex)
        self.assertEqual(len(variables2), 2)
        self.assertIn(x, variables2)
        self.assertIn(y, variables2)

if __name__ == '__main__':
    #debug.reexecute_if_unbuffered()
    unittest.main() 