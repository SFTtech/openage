# Instructions for macOS users

## Prerequisite steps
- XCode >= Xcode 12
- Install [Homebrew](http://brew.sh). If you use some other package managers, you're on your own :)

```
brew update-reset && brew update
brew install --cask font-dejavu
brew install cmake python3 libepoxy freetype fontconfig harfbuzz opus opusfile qt6 libogg libpng toml11 eigen
brew install llvm
pip3 install --upgrade --break-system-packages cython numpy mako lz4 pillow pygments setuptools toml
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

## Building

We advise against using the clang version that comes with macOS (Apple Clang) as it notoriously out of date and often causes compilation errors. Use homebrew's clang if you don't want any trouble. You can pass the path of homebrew clang to the openage `configure` script which will generate the CMake files for building:

```
# on Intel macOS, llvm is by default in /usr/local/Cellar/llvm/bin/
# on ARM macOS, llvm is by default in /opt/homebrew/Cellar/llvm/bin/
./configure --compiler="$(brew --prefix llvm)/bin/clang++" --download-nyan
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
