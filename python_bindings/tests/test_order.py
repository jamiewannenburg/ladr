import unittest
import ladr.order

class TestOrder(unittest.TestCase):
    def test_ordertype_enum(self):
        """Test the Ordertype enum values"""
        self.assertEqual(ladr.order.Ordertype.NOT_COMPARABLE, 0)
        self.assertEqual(ladr.order.Ordertype.SAME_AS, 1)
        self.assertEqual(ladr.order.Ordertype.LESS_THAN, 2)
        self.assertEqual(ladr.order.Ordertype.GREATER_THAN, 3)
        self.assertEqual(ladr.order.Ordertype.LESS_THAN_OR_SAME_AS, 4)
        self.assertEqual(ladr.order.Ordertype.GREATER_THAN_OR_SAME_AS, 5)
        self.assertEqual(ladr.order.Ordertype.NOT_LESS_THAN, 6)
        self.assertEqual(ladr.order.Ordertype.NOT_GREATER_THAN, 7)
    
    def test_compare_vecs(self):
        """Test the compare_vecs function"""
        # Test with equal vectors
        self.assertEqual(
            ladr.order.compare_vecs([1, 2, 3], [1, 2, 3]), 
            ladr.order.Ordertype.SAME_AS
        )
        
        # Test with less than
        self.assertEqual(
            ladr.order.compare_vecs([1, 2, 3], [1, 2, 4]), 
            ladr.order.Ordertype.LESS_THAN
        )
        
        # Test with greater than
        self.assertEqual(
            ladr.order.compare_vecs([1, 2, 4], [1, 2, 3]), 
            ladr.order.Ordertype.GREATER_THAN
        )
        
        # Test with not comparable
        self.assertEqual(
            ladr.order.compare_vecs([1, 2, 3], [1, 2, 3, 4]), 
            ladr.order.Ordertype.NOT_COMPARABLE
        )
    
    def test_copy_vec(self):
        """Test the copy_vec function"""
        original = [1, 2, 3, 4, 5]
        copy = ladr.order.copy_vec(original)
        
        # Check that the copy is identical
        self.assertEqual(original, copy)
        
        # Check that modifying the copy doesn't affect the original
        copy[0] = 10
        self.assertNotEqual(original, copy)
        self.assertEqual(original[0], 1)
        self.assertEqual(copy[0], 10)
    
    def test_merge_sort(self):
        """Test the merge_sort function"""
        # Test with integers
        test_list = [3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5]
        
        def compare(a, b):
            if a < b:
                return ladr.order.Ordertype.LESS_THAN
            elif a > b:
                return ladr.order.Ordertype.GREATER_THAN
            else:
                return ladr.order.Ordertype.SAME_AS
        
        sorted_list = ladr.order.merge_sort(test_list, compare)
        self.assertEqual(sorted_list, [1, 1, 2, 3, 3, 4, 5, 5, 5, 6, 9])
        
        # Test with strings
        test_strings = ["banana", "apple", "cherry", "date"]
        
        def compare_strings(a, b):
            if a < b:
                return ladr.order.Ordertype.LESS_THAN
            elif a > b:
                return ladr.order.Ordertype.GREATER_THAN
            else:
                return ladr.order.Ordertype.SAME_AS
        
        sorted_strings = ladr.order.merge_sort(test_strings, compare_strings)
        self.assertEqual(sorted_strings, ["apple", "banana", "cherry", "date"])

if __name__ == '__main__':
    unittest.main() 