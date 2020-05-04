#!/bin/bash

# This file was taken from dosbox-staging,
# Copyright (c) 2019-2020 Kevin R Croft <krcroft@gmail.com>
# It's licensed under the terms of the GPL-2.0-or-later license.
# Modifications Copyright 2020-2020 the openage authors.
# See copying.md for further legal info.

# This script creates a simple wrapper for tar that performs as follows
# when run under a GitHub workflow:
#   * always returns success
#   * replaces gzip with zstd
#
# Usage: install-tar-wrapper.sh /path/of/real/tar /path/to/wrapper/tar
#
# The purpose of the tar-wrapper is to work-around two issues with
# GitHub's cache implementation:
#   * GitHub honors tar's non-zero return code even if all
#     files and directories were created from hot-cache correctly.
#   * GitHub only uses gzip with standard compression, which is
#     less performant than zstd in all three dimensions:
#       - compression ratio
#       - compression rate
#       - decompression rate
#
# In the case of long-lived caches, we want great compression and
# fast extraction.  Zstandard gives us 500+MB/s extraction regardless
# of the amount of compression achieved, so we simply maximize our
# compression at creation-time.
#
# zstd_opts="-T0 --adapt --long=31 --no-progress -v" can be exchanged with
# zstd_opts="-T0 -19 --long=31 --no-progress -v" (or even up to --ultra -22)
# for even more compression

set -euo pipefail

# Check arguments
if [[ "$#" -ne 2 ]]; then
  echo "Usage: $0 /path/of/real/tar /path/to/wrapper/tar"
  exit 1
fi
tar_bin="$1"
wrapper="$2"
zstd_opts="-T0 --adapt --long=31 --no-progress -v"

cat << EOF > "${wrapper}"
#!/bin/bash
set -euo pipefail
rcode=0
# If we're running outside of GitHub then run tar normally
if [[ -z "\${GITHUB_WORKFLOW:-}" ]]; then
    "${tar_bin}" "\$@"
    rcode="\$?"
# Otherwise use zstd and ignore tar's return-code
else
    first_arg="\${1/z/}"
    shift
    [[ "\${first_arg}" == "-x" ]] && decomp="-d" || decomp=""
    set +e -x
    "${tar_bin}" \\
        --warning=none \\
        --use-compress-program="zstd ${zstd_opts} \${decomp}" \\
        "\${first_arg}" "\$@"
fi
exit "\${rcode}"
EOF
chmod +x "${wrapper}"
