# Building and Testing Python Bindings

## Prerequisites

1. **Python 3.7+** (you have Python 3.12.3 ✓)
2. **CMake 3.18+**
3. **C++ compiler** (GCC/Clang)
4. **Python dependencies:**
   - `pybind11>=2.10.0` (you have 2.11.1 ✓)
   - `scikit-build-core>=0.5.0` (needs to be installed)
   - For testing: `pytest` (optional, some tests use it)

## Installation Steps

### 1. Install Build Dependencies

```bash
pip install scikit-build-core
```

Or if you prefer to use a virtual environment (recommended):

```bash
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install scikit-build-core pybind11
```

### 2. Build the Python Bindings

Navigate to the `python_bindings` directory and build:

```bash
cd python_bindings
pip install -e .
```

Or for a regular installation:

```bash
pip install .
```

The build system will:
- Use CMake to compile the C++ extensions
- Link against the LADR library from `../ladr/`
- Create the `ladr_bindings` Python module
- Install it in development mode (with `-e`) or regular mode

### 3. Alternative: Build with CMake Directly

If you prefer to use CMake directly:

```bash
cd python_bindings
mkdir -p build
cd build
cmake ..
make
```

The compiled module will be in `build/ladr_bindings.cpython-*.so` (or `.pyd` on Windows).

To use it, you'll need to add the build directory to your Python path or copy the module to the `ladr` package directory.

## Testing

The project includes several test files in the `tests/` directory:

- `test_term.py` - Tests for term operations (uses unittest)
- `test_symbols.py` - Tests for symbol table (uses unittest)
- `test_parse.py` - Tests for parsing (uses pytest)
- `test_order.py` - Tests for ordering (uses unittest)
- `test_sympy_term.py` - Tests for SymPy-like interface (uses unittest)
- `test_api_examples.py` - API usage examples (uses pytest)
- `test_error_handling.py` - Error handling tests (uses pytest)
- `test_syntax.py` - Syntax variant tests
- `test_symbol_table.py` - Symbol table consistency tests

### Running Tests

#### Option 1: Using pytest (recommended)

First install pytest if not already installed:

```bash
pip install pytest
```

Then run all tests:

```bash
cd python_bindings
pytest tests/
```

Run a specific test file:

```bash
pytest tests/test_term.py
```

Run with verbose output:

```bash
pytest tests/ -v
```

#### Option 2: Using unittest

Some tests can be run directly with Python:

```bash
cd python_bindings
python3 -m unittest tests.test_term
python3 -m unittest tests.test_symbols
python3 -m unittest tests.test_order
python3 -m unittest tests.test_sympy_term
```

Or run all unittest-based tests:

```bash
python3 -m unittest discover -s tests -p "test_*.py"
```

#### Option 3: Run individual test files

Some test files can be executed directly:

```bash
cd python_bindings
python3 tests/test_term.py
python3 tests/test_symbols.py
python3 tests/test_syntax.py
python3 tests/test_symbol_table.py
```

## Troubleshooting

### Build Issues

1. **CMake not found**: Install CMake (`sudo apt-get install cmake` on Ubuntu/Debian)

2. **pybind11 not found**: Install it with `pip install pybind11`

3. **Linking errors**: Make sure the main LADR library is built first:
   ```bash
   cd /home/jamie/ladr
   make all
   # or
   ./run_cmake.sh
   ```

4. **Import errors after build**: Make sure you're in the correct directory or have installed the package:
   ```bash
   cd python_bindings
   pip install -e .
   ```

### Test Issues

1. **Module not found**: Make sure the bindings are built and installed:
   ```bash
   cd python_bindings
   pip install -e .
   ```

2. **pytest not found**: Install it with `pip install pytest`

3. **Some tests fail**: Check if the LADR library is properly built and the Python bindings are correctly installed.

## Quick Start Example

```bash
# 1. Install dependencies
pip install scikit-build-core pytest

# 2. Build the bindings
cd python_bindings
pip install -e .

# 3. Run tests
pytest tests/ -v
```

## Development Mode

For development, install in editable mode so changes to Python files take effect immediately:

```bash
pip install -e .
```

Note: C++ changes still require recompilation.

