from ladr.term_wrapper import variables, constants, binary, nary
import gc
import sys

def test_operators():
    """Test arithmetic operators."""
    print("\nRunning test_operators")
    x, y = variables('x y')
    a, b = constants('a b')
    
    add_xy = x + y
    print(f"Created: {add_xy}, symbol: {add_xy.symbol}, arity: {add_xy.arity}")
    
    mul_ab = a * b
    print(f"Created: {mul_ab}, symbol: {mul_ab.symbol}, arity: {mul_ab.arity}")
    
    # Also test division to make sure it works
    div_ab = a / b
    print(f"Created: {div_ab}, symbol: {div_ab.symbol}, arity: {div_ab.arity}")
    
    print("test_operators completed successfully")

def test_usage():
    """Test function application."""
    print("\nRunning test_usage")
    x, y = variables('x y')
    
    f = binary('f')
    g = nary('g', 3)
    
    t1 = f(x, y)
    print(f"Created: {t1}, symbol: {t1.symbol}, arity: {t1.arity}")
    
    t2 = g(x, y, x)
    print(f"Created: {t2}, symbol: {t2.symbol}, arity: {t2.arity}")
    
    t3 = f(t1, t2)
    print(f"Created: {t3}, symbol: {t3.symbol}, arity: {t3.arity}")
    
    print("test_usage completed successfully")

if __name__ == "__main__":
    try:
        print("Starting final test sequence")
        
        # Run first test
        test_operators()
        
        # Force garbage collection between tests
        print("\nForcing garbage collection")
        gc.collect()
        print("Garbage collected")
        
        # Run second test
        test_usage()
        
        print("\nAll tests completed successfully!")
        sys.stdout.flush()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1) 