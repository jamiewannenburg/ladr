import subprocess
import sys
import os

def run_test(name):
    """Run a test script in a separate process to avoid memory issues."""
    print(f"\nRunning test: {name}")
    script = f"""
from ladr.term_wrapper import variables, constants, binary, nary

def {name}():
    print("Starting {name}")
    if "{name}" == "test_operators":
        x, y = variables('x y')
        a, b = constants('a b')
        add_xy = x + y
        print(f"Created: {{add_xy}}, symbol: {{add_xy.symbol}}, arity: {{add_xy.arity}}")
        div_ab = a / b
        print(f"Created: {{div_ab}}, symbol: {{div_ab.symbol}}, arity: {{div_ab.arity}}")
    else:  # test_usage
        x, y = variables('x y')
        f = binary('f')
        g = nary('g', 3)
        t1 = f(x, y)
        print(f"Created: {{t1}}, symbol: {{t1.symbol}}, arity: {{t1.arity}}")
        t2 = g(x, y, x)
        print(f"Created: {{t2}}, symbol: {{t2.symbol}}, arity: {{t2.arity}}")
        t3 = f(t1, t2)
        print(f"Created: {{t3}}, symbol: {{t3.symbol}}, arity: {{t3.arity}}")
    print("{name} completed successfully")

{name}()
print("Clean exit")
"""
    
    # Write the script to a temporary file
    temp_file = f"{name}_temp.py"
    with open(temp_file, "w") as f:
        f.write(script)
    
    # Run the script in a separate process
    subprocess.run([sys.executable, temp_file])
    
    # Clean up
    os.remove(temp_file)

print("Running tests in separate processes")
run_test("test_operators")
run_test("test_usage")
print("All tests completed!") 