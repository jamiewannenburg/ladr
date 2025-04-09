from skbuild import setup

setup(
    name="ladr-string",
    version="0.1.0",
    packages=["ladr_string"],
    package_dir={"": "."},
    cmake_args=["-DCMAKE_BUILD_TYPE=Release"],
    cmake_source_dir=".",
    cmake_install_dir="ladr_string",
    python_requires=">=3.7",
)
