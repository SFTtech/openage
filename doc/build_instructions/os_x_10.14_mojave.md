# Instructions for Mac OS X users (OS X 10.14 Mojave)

## Prerequisite steps
- XCode >= Xcode 10.2.1 (Build: 10E1001)
- Install [Homebrew](http://brew.sh). If you use some other package managers, you're on your own :)

```
brew update-reset && brew update
brew tap homebrew/cask-fonts
brew cask install font-dejavu-sans
brew install cmake python3 libepoxy freetype fontconfig harfbuzz sdl2 sdl2_image opus opusfile qt5 libogg libpng eigen
brew install llvm
pip3 install cython numpy jinja2 lz4 pillow pygments toml
```

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies:

```
brew install flex make
```

## Building

```
./configure --compiler=clang++ --mode=release --download-nyan
make
```
## Testing
`make test` runs the built-in tests.

## Running
`make run` or `./bin/run` launches the game. Try `./bin/run --help`!
