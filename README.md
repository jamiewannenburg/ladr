We put -lm to the end of the prover gcc commands so it can be compiled in Ubuntu 24.04.

We changed uses of c++ reserved keywords.

-------

<p align="center">
  <img width=512 src="imgs/prover9-5a-512t.gif">
</p>
<p align="center">
  <img width=512 src="imgs/prover9t.gif">
</p>

# Prover9 and Mace4

Prover9 is an automated theorem prover for first-order and equational logic, and Mace4 searches for finite models and counterexamples. Prover9 is the successor of the [Otter prover](http://www.cs.unm.edu/~mccune/otter/).

## Building from source

Both `CMake` and `make` are supported. Cygwin is recommended for building on Windows (the executables will depend on `cygwin1.dll`). To build using `CMake` (out-of-source) run

```bash
# if this fails, run `chmod +x run_cmake.sh` and try again
./run_cmake.sh
```

To build using `make` (in-source) run

```bash
make all
```

In the end, the binaries will available in the `bin` folder.

### Troubleshooting

If `make` complains about the `round` function not being found (or something else having to do with `math.h`), try moving the `-lm` parameters in [`provers.src/Makefile`](https://github.com/laitep/ladr/blob/main/provers.src/Makefile) to the end of the command, or use `CMake` instead.

If Cygwin complains with `$'\r': command not found`, run [`dos2unix`](https://www.cygwin.com/packages/summary/dos2unix.html) on the `run_cmake.sh` script and try again.

## Setting up MSYS2 with MinGW64 and gnulib

To set up MSYS2 with MinGW64 and gnulib, follow these steps:

1. Install MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)
2. Open the MSYS2 MinGW64 terminal
3. Update the package database and core packages:
   ```bash
   pacman -Syu
   ```
4. Install required development tools:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-autoconf mingw-w64-x86_64-automake mingw-w64-x86_64-libtool git
   ```
5. Clone the gnulib repository:
   ```bash
   git clone git://git.savannah.gnu.org/gnulib.git
   ```
6. Add gnulib to your PATH:
   ```bash
   export PATH=$PATH:$(pwd)/gnulib
   ```
7. You can now use gnulib tools in your projects. For example, to bootstrap a project:
   ```bash
   gnulib-tool --import --dir=. --lib=libgnu --source-base=lib --m4-base=m4 --doc-base=doc --tests-base=tests --aux-dir=build-aux --makefile-name=gnulib.mk --no-conditional-dependencies --no-libtool --macro-prefix=gl --no-vc-files <list of modules>
   ```

Note: Make sure to run these commands in the MSYS2 MinGW64 terminal, not the regular MSYS2 terminal, to ensure proper MinGW64 environment setup.

## Pre-built binaries:

- [The GUI: Prover9 and Mace4 with a Graphical User Interface](https://github.com/laitep/Prover9-Mace4-v05/releases/tag/v1.0.0)
- [LADR: Command-line versions of Prover9, Mace4, and other programs (source only)](https://github.com/laitep/ladr/releases/tag/v1.0.0)

## Other Useful Links

- [Manual and Examples](https://laitep.github.io/ladr/)
- Help! If you have questions about Prover9/Mace4/LADR, please file a new [issue](https://github.com/laitep/ladr/issues/new).
- [Citing the programs in your publications](#citing-prover9-and-mace4)
- [Double-check your Prover9 Proofs with Ivy](http://www.cs.unm.edu/~mccune/ivy_check_prover9/)
- Turn on notifications (using the [`Watch`](https://github.com/laitep/ladr) button on the LADR GitHub page) if you would like to be notified of updates, bugs, etc.

**You should ignore any rumors that Prover9 is part of [a bigger plan](imgs/plan-9-from-outer-space.jpg).**

# Citing Prover9 and Mace4

Please use something like this:

```
W. McCune, LaiTeP contributors, "Prover9 and Mace4", https://github.com/laitep/ladr, 2005-2010, 2023.
```

Bibtex item:

```
@unpublished{ prover9-mace4,
    author = "W. McCune, {LaiTeP} contributors",
    title = "Prover9 and Mace4",
    note = "\verb|https://github.com/laitep/ladr|",
    year = "2005-2010, 2023"}
```

---

_This material is based upon work supported by the [National Science Foundation](http://www.nsf.gov) under Grant No. 0708218._

_Any opinions, findings and conclusions or recomendations expressed in this material are those of the author(s) and do not necessarily reflect the views of the National Science Foundation._
