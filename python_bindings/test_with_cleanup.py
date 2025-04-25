from ladr.term_wrapper import variables, constants, binary, nary
import gc
import sys

def create_and_destroy():
    """Create terms and explicitly destroy them with garbage collection."""
    print("\nStep 1: Creating variables")
    x, y = variables('x y')
    print(f"Created variables: x={x}, y={y}")
    
    print("\nStep 2: Creating binary term")
    add_xy = x + y
    print(f"Created term: {add_xy}")
    
    print("\nStep 3: Force garbage collection")
    # Clear references to allow garbage collection
    del x, y, add_xy
    gc.collect()
    print("Garbage collected")
    
    print("\nStep 4: Creating new variables")
    a, b = variables('a b')
    print(f"Created variables: a={a}, b={b}")
    
    print("\nStep 5: Creating function symbols")
    f = binary('f')
    g = nary('g', 3)
    print(f"Created functions: f (arity={f.arity}), g (arity={g.arity})")
    
    print("\nStep 6: Creating complex terms")
    t1 = f(a, b)
    print(f"Created t1: {t1}")
    t2 = g(a, b, a)
    print(f"Created t2: {t2}")
    t3 = f(t1, t2)
    print(f"Created t3: {t3}")
    
    print("\nStep 7: Force garbage collection")
    del a, b, f, g, t1, t2, t3
    gc.collect()
    print("Garbage collected")
    
    print("\nTest completed")

if __name__ == "__main__":
    try:
        print("Starting test with explicit garbage collection")
        create_and_destroy()
        print("\nAll operations completed successfully")
        sys.stdout.flush()  # Ensure all output is flushed before exiting
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1) 