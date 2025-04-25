from ladr.term_wrapper import variables, constants, binary, nary
import gc

# Test case 1 - Using only addition and multiplication (no subtraction or negation)
def test_safe_operators():
    print("Running test_safe_operators")
    x, y = variables('x y')
    a, b = constants('a b')
    add_xy = x + y
    print(f"Created: {add_xy}, symbol: {add_xy.symbol}, arity: {add_xy.arity}")
    mul_ab = a * b
    print(f"Created: {mul_ab}, symbol: {mul_ab.symbol}, arity: {mul_ab.arity}")
    print("Safe operators test finished")

# Test case 2 - Function application with custom symbols
def test_custom_functions():
    print("Running test_custom_functions")
    x, y = variables('x y')
    f = binary('f')
    g = nary('g', 3)
    t1 = f(x, y)
    print(f"Created: {t1}, symbol: {t1.symbol}, arity: {t1.arity}")
    t2 = g(x, y, x)
    print(f"Created: {t2}, symbol: {t2.symbol}, arity: {t2.arity}")
    t3 = f(t1, t2)
    print(f"Created: {t3}, symbol: {t3.symbol}, arity: {t3.arity}")
    print("Custom functions test finished")

# Run tests in sequence
print("Starting test sequence")

# Run first test
test_safe_operators()

print("Forcing garbage collection")
gc.collect()

# Run second test
print("\nStarting second test")
test_custom_functions()

print("All tests completed successfully") 