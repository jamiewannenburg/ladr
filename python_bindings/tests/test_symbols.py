import unittest
from ladr import symbols
from ladr import order

class TestSymbols(unittest.TestCase):
    def setUp(self):
        # Initialize the symbol table before each test
        symbols.declare_base_symbols()
        symbols.declare_standard_symbols()

    def test_str_to_sn_and_back(self):
        """Test string to symbol number conversion and back"""
        # Test with a standard symbol
        symnum = symbols.str_to_sn("f", 2)  # Binary function
        self.assertGreater(symnum, 0)
        self.assertEqual(symbols.sn_to_str(symnum), "f")
        self.assertEqual(symbols.sn_to_arity(symnum), 2)

        # Test with a constant
        symnum = symbols.str_to_sn("a", 0)  # Constant
        self.assertGreater(symnum, 0)
        self.assertEqual(symbols.sn_to_str(symnum), "a")
        self.assertEqual(symbols.sn_to_arity(symnum), 0)

    def test_parse_type_functions(self):
        """Test parse type setting and checking"""
        # Declare a new symbol
        symnum = symbols.str_to_sn("test_op", 2)
        
        # Set parse type
        symbols.set_parse_type("test_op", 100, symbols.ParseType.INFIX)
        
        # Check if the symbol exists with correct properties
        self.assertTrue(symbols.is_symbol(symnum, "test_op", 2))

    def test_symbol_type_functions(self):
        """Test symbol type setting and checking"""
        # Create a symbol
        symnum = symbols.str_to_sn("test_func", 2)
        
        # Set as function
        symbols.set_symbol_type(symnum, symbols.SymbolType.FUNCTION)
        self.assertEqual(symbols.get_symbol_type(symnum), symbols.SymbolType.FUNCTION)
        self.assertTrue(symbols.function_symbol(symnum))
        self.assertFalse(symbols.relation_symbol(symnum))

        # Set as relation
        symbols.set_symbol_type(symnum, symbols.SymbolType.RELATION)
        self.assertEqual(symbols.get_symbol_type(symnum), symbols.SymbolType.RELATION)
        self.assertFalse(symbols.function_symbol(symnum))
        self.assertTrue(symbols.relation_symbol(symnum))

    def test_variable_style(self):
        """Test variable style setting and checking"""
        # Test default style
        self.assertEqual(symbols.variable_style(), symbols.VariableStyle.STANDARD)
        
        # Change style
        symbols.set_variable_style(symbols.VariableStyle.PROLOG)
        self.assertEqual(symbols.variable_style(), symbols.VariableStyle.PROLOG)
        
        # Test variable name checking
        self.assertTrue(symbols.variable_name("X"))
        self.assertTrue(symbols.variable_name("Y1"))
        self.assertFalse(symbols.variable_name("x"))  # Lowercase not allowed in Prolog style

    def test_skolem_functions(self):
        """Test skolem symbol functions"""
        # Create a skolem symbol
        symnum = symbols.next_skolem_symbol(2)  # Binary skolem
        self.assertGreater(symnum, 0)
        
        # Check skolem properties
        self.assertTrue(symbols.is_skolem(symnum))
        self.assertEqual(symbols.sn_to_arity(symnum), 2)

    def test_lexicographic_ordering(self):
        """Test lexicographic ordering functions"""
        # Create two symbols
        symnum1 = symbols.str_to_sn("a", 0)
        symnum2 = symbols.str_to_sn("b", 0)
        
        # Set lex values
        symbols.set_lex_val(symnum1, 1)
        symbols.set_lex_val(symnum2, 2)
        
        # Check values
        self.assertEqual(symbols.sn_to_lex_val(symnum1), 1)
        self.assertEqual(symbols.sn_to_lex_val(symnum2), 2)
        
        # Compare precedence
        self.assertEqual(symbols.sym_precedence(symnum1, symnum2), order.OrderType.LESS_THAN)
        self.assertEqual(symbols.sym_precedence(symnum2, symnum1), order.OrderType.GREATER_THAN)
        self.assertEqual(symbols.sym_precedence(symnum1, symnum1), order.OrderType.SAME_AS)

    def test_kb_ordering(self):
        """Test Knuth-Bendix ordering functions"""
        # Create a symbol
        symnum = symbols.str_to_sn("test_kb", 2)
        
        # Set KB weight
        symbols.set_kb_weight(symnum, 5)
        
        # Check weight
        self.assertEqual(symbols.sn_to_kb_wt(symnum), 5)

    def test_lrpo_functions(self):
        """Test LRPO functions"""
        # Create a symbol
        symnum = symbols.str_to_sn("test_lrpo", 2)
        
        # Set LRPO status
        symbols.set_lrpo_status(symnum, symbols.LrpoStatus.LRPO_MUL)
        
        # Check status
        self.assertEqual(symbols.sn_to_lrpo_status(symnum), symbols.LrpoStatus.LRPO_MUL)

    def test_assoc_comm_functions(self):
        """Test associative-commutative functions"""
        # Create a symbol
        symnum = symbols.str_to_sn("test_ac", 2)
        
        # Set AC property
        symbols.set_assoc_comm("test_ac", True)
        self.assertTrue(symbols.is_assoc_comm(symnum))
        
        # Set commutative property
        symbols.set_commutative("test_ac", True)
        self.assertTrue(symbols.is_commutative(symnum))
        
        # Remove properties
        symbols.set_assoc_comm("test_ac", False)
        symbols.set_commutative("test_ac", False)
        self.assertFalse(symbols.is_assoc_comm(symnum))
        self.assertFalse(symbols.is_commutative(symnum))


if __name__ == '__main__':
    unittest.main() 