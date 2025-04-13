import unittest
import ladr.mace4 as mace4
from mace4.mace4 import Mace4

class TestMace4(unittest.TestCase):
    def setUp(self):
        self.mace = Mace4()

    def test_initialization(self):
        """Test that Mace4 initializes correctly with default options."""
        self.assertIsNotNone(self.mace.options)
        self.assertIsNotNone(self.mace.stats)
        self.assertIsNone(self.mace.get_results())

    def test_set_options(self):
        """Test setting options for Mace4."""
        self.mace.set_options(domain_size=3, max_models=1, max_seconds=10)
        self.assertEqual(self.mace.options.domain_size, 3)
        self.assertEqual(self.mace.options.max_models, 1)
        self.assertEqual(self.mace.options.max_seconds, 10)

    def test_run_with_empty_clauses(self):
        """Test running Mace4 with an empty list of clauses."""
        result = self.mace.run([])
        self.assertIsNotNone(result)
        self.assertIsInstance(result, mace4.MaceResults)
        self.assertFalse(result.success)  # Expect failure with empty clauses

    def test_run_with_sample_clauses(self):
        """Test running Mace4 with a sample list of clauses."""
        # Note: Actual clauses would need to be properly formatted terms or formulas
        # For demonstration, we're passing a dummy list since conversion is not yet implemented
        clauses = []  # Replace with actual clause data once conversion is implemented
        self.mace.set_options(domain_size=2, max_models=1)
        result = self.mace.run(clauses)
        self.assertIsNotNone(result)
        self.assertIsInstance(result, mace4.MaceResults)

if __name__ == '__main__':
    unittest.main()
