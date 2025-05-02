import unittest
from ladr import order

class TestOrder(unittest.TestCase):
    def setUp(self):
        # Initialize any test data here
        pass

    def test_order_type_values(self):
        """Test that all OrderType enum values are accessible"""
        self.assertEqual(order.OrderType.NOT_COMPARABLE, 0)
        self.assertEqual(order.OrderType.SAME_AS, 1)
        self.assertEqual(order.OrderType.LESS_THAN, 2)
        self.assertEqual(order.OrderType.GREATER_THAN, 3)
        self.assertEqual(order.OrderType.LESS_THAN_OR_SAME_AS, 4)
        self.assertEqual(order.OrderType.GREATER_THAN_OR_SAME_AS, 5)
        self.assertEqual(order.OrderType.NOT_LESS_THAN, 6)
        self.assertEqual(order.OrderType.NOT_GREATER_THAN, 7)

    def test_compare_vecs(self):
        """Test vector comparison"""
        # Test equal vectors
        vec1 = [1, 2, 3]
        vec2 = [1, 2, 3]
        self.assertEqual(order.compare_vecs(vec1, vec2), order.OrderType.SAME_AS)

        # Test less than
        vec1 = [1, 2, 3]
        vec2 = [1, 2, 4]
        self.assertEqual(order.compare_vecs(vec1, vec2), order.OrderType.LESS_THAN)

        # Test greater than
        vec1 = [1, 2, 4]
        vec2 = [1, 2, 3]
        self.assertEqual(order.compare_vecs(vec1, vec2), order.OrderType.GREATER_THAN)

        # Test different lengths (should raise exception)
        vec1 = [1, 2, 3]
        vec2 = [1, 2]
        with self.assertRaises(RuntimeError):
            order.compare_vecs(vec1, vec2)

    def test_copy_vec(self):
        """Test vector copying"""
        # Test copying equal length vectors
        vec1 = [1, 2, 3]
        vec2 = [0, 0, 0]
        order.copy_vec(vec1, vec2)
        self.assertEqual(vec2, [1, 2, 3])

        # Test copying different lengths (should raise exception)
        vec1 = [1, 2, 3]
        vec2 = [0, 0]
        with self.assertRaises(RuntimeError):
            order.copy_vec(vec1, vec2)

if __name__ == '__main__':
    unittest.main() 