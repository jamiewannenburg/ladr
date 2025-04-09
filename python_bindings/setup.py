from skbuild import setup

setup(
    name="ladr",
    version="0.1.0",
    packages=["ladr"],
    package_dir={"": "."},
    cmake_args=["-DCMAKE_BUILD_TYPE=Release"],
    cmake_source_dir=".",
    cmake_install_dir="ladr",
    python_requires=">=3.7",
)
