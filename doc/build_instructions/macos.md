# Instructions for macOS users

## Prerequisite steps
- XCode >= Xcode 12
- Install [Homebrew](http://brew.sh). If you use some other package managers, you're on your own :)

```
brew update-reset && brew update
brew install --cask font-dejavu
brew install cmake python3 libepoxy freetype fontconfig harfbuzz opus opusfile libogg libpng toml11 eigen
brew install qtbase qtdeclarative qtmultimedia
```

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies:

```
brew install flex make
```

Optionally, for documentation generation:

```
brew install doxygen
```

## Clone the repository

```
git clone https://github.com/SFTtech/openage
cd openage
```

## Python dependencies

Install the Python packages into a virtual environment inside the repository:

```
python3 -m venv .venv
.venv/bin/pip install --upgrade cython numpy mako lz4 pillow pygments setuptools toml
```

## Building

Recent versions of Apple Clang support the C++20 features openage uses, so the
compiler shipped with Xcode is normally fine:

```
./configure --download-nyan -- -DPython3_EXECUTABLE="$PWD/.venv/bin/python"
```

Older Apple Clang releases are missing parts of C++20 and will fail to build.
If you hit compiler errors, install Homebrew's LLVM and point `configure` at it instead:

```
brew install llvm
./configure --compiler="$(brew --prefix llvm)/bin/clang++" --download-nyan -- -DPython3_EXECUTABLE="$PWD/.venv/bin/python"
```

Afterwards, trigger the build using `make`:

```
make -j$(sysctl -n hw.ncpu)
```

## Testing
`make test` runs the built-in tests.


## Running
`make run` or `cd bin && ./run` launches the game. Try `./run --help` if you don't know what to do!


## To create the documentation
`make doc`
For more options and details, refer to [doc/README.md][/doc/README.md]
