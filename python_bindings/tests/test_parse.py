from ladr import parse
import io
import os 
import tempfile

def test_caster():
    logfile = tempfile.NamedTemporaryFile(delete=False)
    infile = io.BytesIO("f(x,y)".encode('ascii'))
    t = parse.sread_term(infile, logfile.name)
    print(t)
    infile.close()
    logfile.close()
    os.remove(logfile.name)
    print("Test passed")
    

if __name__ == "__main__":
    test_caster()
