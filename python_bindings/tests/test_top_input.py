import unittest
from ladr import top_input, parse, formula, term
import debug

class TestTopInput(unittest.TestCase):
    def setUp(self):
        # Initialize standard parse types
        parse.declare_standard_parse_types()
        self.topform = top_input.Topform()
    @debug.traced
    def test_embed_formulas_in_topforms(self):
        """Test that it can covert a list of formulas into a list of topforms"""

        var1 = term.get_variable_term(1)
        var2 = term.get_variable_term(2)
        func = term.term2("=", var1, var2)
        terms = [func]
        formulas = [formula.term_to_formula(t) for t in terms]
        topforms = top_input.embed_formulas_in_topforms(formulas, True)
        self.assertEqual(len(topforms), 2)
        self.assertIsInstance(topforms[0], top_input.Topform)
        self.assertIsInstance(topforms[1], top_input.Topform)

if __name__ == '__main__':
    unittest.main()
