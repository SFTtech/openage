# Instructions for Mac OS X users (OS X 10.14 Mojave)

## Prerequisite steps
- XCode >= Xcode 10.2.1 (Build: 10E1001)
- Install [Homebrew](http://brew.sh). If you use some other package managers, you're on your own :)

```
brew update-reset && brew update
brew tap homebrew/cask-fonts
brew install --cask font-dejavu-sans
brew install cmake python3 libepoxy freetype fontconfig harfbuzz sdl2 sdl2_image opus opusfile qt5 libogg libpng eigen
brew install llvm
pip3 install cython numpy jinja2 lz4 pillow pygments toml
```
Install this **optionally** to later generate the documentation

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies:

```
brew install flex make
```

(For noobs
## Clone the repository
```
git clone https://github.com/SFTtech/openage
cd openage
```
)

## Building

```
./configure --compiler=clang++ --mode=release --download-nyan
make
```
## Testing
`make test` runs the built-in tests.

## Running
`make run` or `./bin/run` launches the game. Try `./bin/run --help`!

## To create the documentation
`make doc`
For more options and details, refer to [doc/README.md][/doc/README.md]
