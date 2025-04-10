import unittest
import ladr.header

class TestHeader(unittest.TestCase):
    def test_bool_enum(self):
        """Test the BOOL enum values"""
        self.assertEqual(ladr.header.BOOL.TRUE, 1)
        self.assertEqual(ladr.header.BOOL.FALSE, 0)
        
    def test_imax_imin(self):
        """Test the IMAX and IMIN functions"""
        # Test with positive integers
        self.assertEqual(ladr.header.imax(5, 10), 10)
        self.assertEqual(ladr.header.imin(5, 10), 5)
        
        # Test with negative integers
        self.assertEqual(ladr.header.imax(-5, -10), -5)
        self.assertEqual(ladr.header.imin(-5, -10), -10)
        
        # Test with equal values
        self.assertEqual(ladr.header.imax(7, 7), 7)
        self.assertEqual(ladr.header.imin(7, 7), 7)
        
    def test_dbl_large(self):
        """Test the DBL_LARGE constant"""
        self.assertEqual(ladr.header.DBL_LARGE, 10E11)

if __name__ == '__main__':
    unittest.main() 