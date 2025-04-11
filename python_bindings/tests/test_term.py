import unittest
import ladr.term
import ladr.header

class TestTerm(unittest.TestCase):
    def test_constants(self):
        """Test the term module constants"""
        self.assertIsInstance(ladr.term.MAX_VARS, int)
        self.assertIsInstance(ladr.term.MAX_VNUM, int)
        self.assertIsInstance(ladr.term.MAX_VAR, int)
        self.assertIsInstance(ladr.term.MAX_SYM, int)
        self.assertIsInstance(ladr.term.MAX_ARITY, int)
    
    def test_variable_term(self):
        """Test variable term creation and properties"""
        var = ladr.term.get_variable_term(1)
        self.assertTrue(var.is_variable)
        self.assertFalse(var.is_constant)
        self.assertFalse(var.is_complex)
        self.assertEqual(var.varnum, 1)
        self.assertEqual(var.arity, 0)
        self.assertEqual(str(var), "v1")
        self.assertEqual(len(var), 0)
        
        with self.assertRaises(ValueError):
            _ = var.symnum  # Variables don't have symnum
    
    def test_constant_term(self):
        """Test constant term creation and properties"""
        const = ladr.term.term0("a")
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
    
    def test_complex_term(self):
        """Test complex term creation and properties"""
        var1 = ladr.term.get_variable_term(1)
        var2 = ladr.term.get_variable_term(2)
        func = ladr.term.term2("f", var1, var2)
        
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
    
    def test_term_operations(self):
        """Test term operations"""
        var = ladr.term.get_variable_term(1)
        const = ladr.term.term0("a")
        func = ladr.term.term1("f", var)
        
        # Test copy_term
        var_copy = ladr.term.copy_term(var)
        self.assertTrue(var_copy.is_variable)
        self.assertEqual(var_copy.varnum, 1)
        
        # Test term_ident
        self.assertEqual(ladr.term.term_ident(var, var_copy), ladr.header.BOOL.TRUE)
        self.assertEqual(ladr.term.term_ident(var, const), ladr.header.BOOL.FALSE)
        
        # Test is_term
        self.assertTrue(ladr.term.is_term(func, "f", 1))
        self.assertFalse(ladr.term.is_term(func, "g", 1))
        self.assertFalse(ladr.term.is_term(func, "f", 2))
        
        # Test is_constant
        self.assertTrue(ladr.term.is_constant(const, "a"))
        self.assertFalse(ladr.term.is_constant(const, "b"))
        self.assertFalse(ladr.term.is_constant(func, "f"))
        
        # Test ground_term
        self.assertTrue(ladr.term.ground_term(const))
        self.assertFalse(ladr.term.ground_term(var))
        self.assertFalse(ladr.term.ground_term(func))
        
        # Test term_depth
        self.assertEqual(ladr.term.term_depth(const), 0)
        self.assertEqual(ladr.term.term_depth(var), 0)
        self.assertEqual(ladr.term.term_depth(func), 1)
        
        # Test symbol_count
        self.assertEqual(ladr.term.symbol_count(const), 1)
        self.assertEqual(ladr.term.symbol_count(var), 1)
        self.assertEqual(ladr.term.symbol_count(func), 2)
        
        # Test occurs_in
        self.assertTrue(ladr.term.occurs_in(var, func))
        self.assertFalse(ladr.term.occurs_in(const, func))
    
    def test_term_builders(self):
        """Test term builders"""
        var = ladr.term.get_variable_term(1)
        
        # Test term0, term1, term2
        const = ladr.term.term0("a")
        self.assertTrue(const.is_constant)
        self.assertEqual(const.symbol, "a")
        
        unary = ladr.term.term1("f", var)
        self.assertTrue(unary.is_complex)
        self.assertEqual(unary.symbol, "f")
        self.assertEqual(unary.arity, 1)
        
        binary = ladr.term.term2("g", var, const)
        self.assertTrue(binary.is_complex)
        self.assertEqual(binary.symbol, "g")
        self.assertEqual(binary.arity, 2)
        
        # Test build_unary_term and build_binary_term
        unary2 = ladr.term.build_unary_term("h", var)
        self.assertTrue(unary2.is_complex)
        self.assertEqual(unary2.symbol, "h")
        self.assertEqual(unary2.arity, 1)
        
        binary2 = ladr.term.build_binary_term("k", var, const)
        self.assertTrue(binary2.is_complex)
        self.assertEqual(binary2.symbol, "k")
        self.assertEqual(binary2.arity, 2)
    
    def test_conversions(self):
        """Test conversion functions"""
        # Integer conversions
        int_term = ladr.term.int_to_term(42)
        self.assertTrue(int_term.is_constant)
        self.assertEqual(ladr.term.term_to_int(int_term), 42)
        
        # Natural number conversions
        nat_term = ladr.term.nat_to_term(7)
        self.assertTrue(nat_term.is_constant)
        self.assertEqual(ladr.term.term_to_int(nat_term), 7)
        
        # Boolean conversions
        true_term = ladr.term.bool_to_term(ladr.header.BOOL.TRUE)
        self.assertTrue(true_term.is_constant)
        self.assertEqual(ladr.term.term_to_bool(true_term), True)
        
        false_term = ladr.term.bool_to_term(ladr.header.BOOL.FALSE)
        self.assertTrue(false_term.is_constant)
        self.assertEqual(ladr.term.term_to_bool(false_term), False)
        
        # Test conversion failures
        var = ladr.term.get_variable_term(1)
        with self.assertRaises(ValueError):
            ladr.term.term_to_int(var)
        
        with self.assertRaises(ValueError):
            ladr.term.term_to_bool(var)

if __name__ == '__main__':
    unittest.main() 