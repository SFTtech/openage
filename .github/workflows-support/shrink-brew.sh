#!/bin/bash

# This file was taken from dosbox-staging,
# Copyright (c) 2019-2020 Kevin R Croft <krcroft@gmail.com>
# It's licensed under the terms of the GPL-2.0-or-later license.
# Modifications Copyright 2020-2020 the openage authors.
# See copying.md for further legal info.

# This script reduces the size of a brew-managed installation on macOS.
# Its main use is to shrink this area to fit within GitHub's cache
# limits however it can also be used by end-users wanting to save space.
#
# Note that this script remove alot of content deemed unecessary for our
# purposes such as ruby, go, grade, azure, etc..) so using on your
# home system is probably not what you want :-)
#
# Usage: ./shrink-brew.sh
#
set -xuo pipefail
set +e

# Ensure we have sudo rights
if [[ $(id -u) -ne 0 ]] ; then echo "Please run as root" ; exit 1 ; fi
user="${SUDO_USER}"
group="$(id -g "${user}")"

# If we don't have /usr/local then brew hasn't installed anything yet
cd /usr/local || exit 0

# Purge unecessary bloat
for dir in lib/ruby Cellar/go Cellar/gradle Cellar/azure-cli lib/node_modules \
share/powershell Caskroom/fastlane Cellar/ruby opt/AGPM Caskroom Cellar/node \
miniconda Cellar/python@2 Cellar/git lib/python2.7 Cellar/git-lfs Cellar/subversion \
Cellar/maven Cellar/aria2 Homebrew/Library/Homebrew/os/mac/pkgconfig/fuse \
.com.apple.installer.keep microsoft; do
    rm -rf "${dir}" || true
done

# Cleanup permissions and attributes
chflags nouchg .
find . -type d -exec xattr -c {} +
find . -type f -exec xattr -c {} +
chown -R "${user}:${group}" ./*
find . ! -path . -type d -exec chmod 770 {} +

# Binary-stripping is *extremely* verbose, so we send these to the ether
find Cellar -name '*' -a ! -iname 'strip' -type f -perm +111 -exec strip {} + &> /dev/null
find Cellar -name '*.a' -type f -exec strip {} + &> /dev/null

# Post-cleanup size check
du -sch . 2> /dev/null

# This entire script is best-effort, so always return success
exit 0

