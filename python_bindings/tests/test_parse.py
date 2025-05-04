from ladr import parse, term
import io
import os 
import tempfile
import pytest

class TestParse:
    def setup_method(self):
        self.logfile = tempfile.NamedTemporaryFile(delete=False)

    def teardown_method(self):
        self.logfile.close()
        os.remove(self.logfile.name)

    def test_sread_term(self):
        infile = io.BytesIO("f(x,y)".encode('ascii'))
        t = parse.sread_term(infile, self.logfile.name)
        infile.close()
        assert isinstance(t, term.Term)
        
    # test that the buffer is not consumed past the end character
    def test_sread_term_buffer_stop(self):
        infile = io.BytesIO("f(x,y).more".encode('ascii'))
        t = parse.sread_term(infile, self.logfile.name)
        assert isinstance(t, term.Term)
        # check that only up till . is consumed
        assert infile.read() == b"more"
        infile.close()

if __name__ == "__main__":
    pytest.main()