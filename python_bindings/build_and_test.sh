#!/bin/bash
set -e

# Clean previous builds
rm -rf build/ _skbuild/ dist/

# Install the package in development mode
pip install -e .

# Run the tests
python -m unittest tests.test_fatal -v 