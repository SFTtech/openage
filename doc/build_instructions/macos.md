# Instructions for macOS users

## Prerequisite steps
- XCode >= Xcode 12
- Install [Homebrew](http://brew.sh). If you use some other package managers, you're on your own :)

```
brew update-reset && brew update
brew install --cask font-dejavu
brew install cmake just ninja python3 libepoxy freetype fontconfig harfbuzz opus opusfile libogg libpng toml11 eigen
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
uv sync
```
or, to create the venv manually:
```
python3 -m venv .venv
.venv/bin/pip install --upgrade cython setuptools numpy mako lz4 pillow pygments tomli-w
```

## Building

openage needs C++26 with module support, which Apple Clang does not provide yet.
Install Homebrew's LLVM and point `configure` at it:

```
brew install llvm
./configure --compiler="$(brew --prefix llvm)/bin/clang++" --download-nyan -- -DPython3_EXECUTABLE="$PWD/.venv/bin/python"
```

Afterwards, trigger the build using `just build`:

```
just build
```

## Testing
`just test` runs the built-in tests.


## Running
`just run` or `cd bin && ./run` launches the game. Try `./run --help` if you don't know what to do!


## To create the documentation
`just doc`
