# Instructions for macOS users

## Prerequisite steps
- XCode >= Xcode 12
- Install [Homebrew](http://brew.sh). If you use some other package managers, you're on your own :)

```
brew update-reset && brew update
brew tap homebrew/cask-fonts
brew install font-dejavu
brew install cmake python3 libepoxy freetype fontconfig harfbuzz sdl2 sdl2_image opus opusfile qt6 libogg libpng eigen
brew install llvm
pip3 install cython numpy jinja2 lz4 pillow pygments toml

# optional, for documentation generation
brew install doxygen
```

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies:

```
brew install flex make
```

## Clone the repository
```
git clone https://github.com/SFTtech/openage
cd openage
```

## Building

```
./configure --compiler=$(which clang++) --mode=release --download-nyan
make -j$(sysctl -n hw.ncpu)
```

## Testing
`make test` runs the built-in tests.


## Running
`make run` or `./bin/run` launches the game. Try `./bin/run --help`!


## To create the documentation
`make doc`
For more options and details, refer to [doc/README.md][/doc/README.md]
