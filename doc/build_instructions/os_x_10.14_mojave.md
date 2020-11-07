# Instructions for Mac OS X users (OS X 10.14 Mojave)

## Prerequisite steps
- XCode >= Xcode 10.2.1 (Build: 10E1001)
- Install [Homebrew](http://brew.sh). Other package managers and building from source might also work, but aren't supported as of now. If you figure out fixes for those other options, you're welcome to open pullrequests, as long as the Homebrew build doesn't break.
```
brew update-reset && brew update
brew tap homebrew/cask-fonts
brew cask install font-dejavu-sans
brew install cmake python3 libepoxy freetype fontconfig harfbuzz sdl2 sdl2_image opus libogg opusfile libpng
brew install qt5
brew install -cc=clang llvm@8
export PATH="/usr/local/opt/llvm@8/bin:$PATH:/usr/local/lib:/usr/local/opt/llvm/bin"
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig:/usr/local/lib"
pip3 install pygments cython numpy lz4 pillow pyreadline toml jinja2
```

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies:

```
brew install flex
brew install make
```

## Building

```
./configure --compiler=clang++ --mode=release --download-nyan
make
```
## Testing
`make test` runs the built-in tests.

## Running
`make run` or `./run` launches the game. Try `./run --help`!
