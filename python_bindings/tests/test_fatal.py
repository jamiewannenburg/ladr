import unittest
import sys
import io
import traceback
from contextlib import redirect_stdout, redirect_stderr
import ladr.fatal

class TestFatal(unittest.TestCase):
    def setUp(self):
        # Save original exit code and reset it to default
        self.original_exit_code = ladr.fatal.get_fatal_exit_code()
        ladr.fatal.set_fatal_exit_code(1)

    def tearDown(self):
        # Restore original exit code
        ladr.fatal.set_fatal_exit_code(self.original_exit_code)

    def test_get_set_fatal_exit_code(self):
        # Test setting and getting exit code
        test_code = 42
        ladr.fatal.set_fatal_exit_code(test_code)
        self.assertEqual(ladr.fatal.get_fatal_exit_code(), test_code)

    def test_bell(self):
        try:
            # Test bell function output
            with redirect_stdout(io.StringIO()) as f:
                ladr.fatal.bell()
                output = f.getvalue()
                self.assertEqual(output, '\007')  # \007 is the bell character
        except Exception as e:
            print(f"Error in test_bell: {e}")
            print("Traceback:")
            traceback.print_exc()
            raise

    def test_fatal_error(self):
        # Test fatal_error function
        error_message = "Test fatal error"
        expected_output = f"\nFatal error:  {error_message}\n\n"
        
        # Capture both stdout and stderr
        stdout = io.StringIO()
        stderr = io.StringIO()
        
        # We need to catch SystemExit since fatal_error calls exit()
        with redirect_stdout(stdout), redirect_stderr(stderr), self.assertRaises(SystemExit) as cm:
            ladr.fatal.fatal_error(error_message)
        
        # Check that it exited with the correct code
        self.assertEqual(cm.exception.code, ladr.fatal.get_fatal_exit_code())
        
        # Check that the error message was printed to both stdout and stderr
        self.assertEqual(stdout.getvalue(), expected_output)
        self.assertEqual(stderr.getvalue(), expected_output)

if __name__ == '__main__':
    unittest.main() 