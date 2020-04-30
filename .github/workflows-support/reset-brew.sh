#!/bin/bash

# This file was taken from dosbox-staging,
# Copyright (c) 2019-2020 Kevin R Croft <krcroft@gmail.com>
# It's licensed under the terms of the GPL-2.0-or-later license.
# Modifications Copyright 2020-2020 the openage authors.
# See copying.md for further legal info.

# This script removes all existing brew packages, and then
# re-installs some important basic packages (listed below).
#
# Usage: ./reset-brew.sh
#
set -xuo pipefail
set +e

# Pre-cleanup size
sudo du -sch /usr/local 2> /dev/null

# Uninstall all packages
# shellcheck disable=SC2046
brew remove --force $(brew list) --ignore-dependencies
# shellcheck disable=SC2046
brew cask remove --force $(brew cask list)

# Reinstall important packages
brew install git git-lfs python curl wget jq binutils zstd gnu-tar

# Clean the brew cache
rm -rf "$(brew --cache)"

# Post-clean up size
sudo du -sch /usr/local 2> /dev/null

# This script is best-effort, so always return success
exit 0
