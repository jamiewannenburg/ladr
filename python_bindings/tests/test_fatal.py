import unittest
import sys
import io
import traceback
from contextlib import redirect_stdout, redirect_stderr, contextmanager
import ladr.fatal
from ladr_bindings.errors import FatalError, DEFAULT_FATAL_EXIT_CODE

@contextmanager
def capture_output():
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        try:
            yield stdout, stderr
        finally:
            stdout.seek(0)
            stderr.seek(0)

class TestFatal(unittest.TestCase):
    def setUp(self):
        # Save original exit code and reset it to default
        self.original_exit_code = ladr.fatal.get_fatal_exit_code()
        ladr.fatal.set_fatal_exit_code(DEFAULT_FATAL_EXIT_CODE)

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
            with capture_output() as (stdout, stderr):
                ladr.fatal.bell()
                self.assertEqual(stdout.getvalue(), '\007')  # \007 is the bell character
                self.assertEqual(stderr.getvalue(), '')  # stderr should be empty
        except Exception as e:
            print(f"Error in test_bell: {e}")
            print("Traceback:")
            traceback.print_exc()
            raise

    def test_fatal_error(self):
        # Test fatal_error function
        error_message = "Test fatal error"
        expected_output = f"\nFatal error:  {error_message}\n\n"
        
        # We need to catch FatalError since fatal_error now raises it
        with self.assertRaises(FatalError) as cm:
            with capture_output() as (stdout, stderr):
                try:
                    ladr.fatal.fatal_error(error_message)
                except FatalError as e:
                    # Check outputs before re-raising
                    self.assertEqual(stdout.getvalue(), "")  # stdout should be empty
                    self.assertEqual(stderr.getvalue(), "")  # stderr should be empty
                    raise
                except Exception as e:
                    print(f"Exception: {e}")
                    raise
        
        # Check that the exception has the correct message and exit code
        self.assertEqual(str(cm.exception), error_message)
        self.assertEqual(cm.exception.exit_code, ladr.fatal.get_fatal_exit_code())

if __name__ == '__main__':
    unittest.main() 