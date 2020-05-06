#!/bin/bash

# Set CI environment variables

echo 'export PATH="$(brew --prefix)/opt/llvm/bin:/usr/local/opt/ccache/libexec:$PATH"' >> ~/.bash_profile
source ~/.bash_profile
export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig:/usr/local/lib"
export LDFLAGS="-L$(brew --prefix)/opt/llvm/lib -Wl,-rpath,$(brew --prefix)/opt/llvm/lib,$LDFLAGS"
export CPPFLAGS="-I$(brew --prefix)/opt/llvm/include,$CPPFLAGS"
