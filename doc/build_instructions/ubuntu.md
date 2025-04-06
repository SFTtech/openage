# Prerequisite steps for Ubuntu users (Ubuntu >= 23.04)

Run the following commands:

```bash
 sudo apt-get update
 sudo apt-get install g++ cmake cython3 libeigen3-dev libepoxy-dev libfontconfig1-dev libfreetype-dev libharfbuzz-dev libogg-dev libopus-dev libopusfile-dev libpng-dev libtoml11-dev python3-dev python3-mako python3-numpy python3-lz4 python3-pil python3-pip python3-pygments python3-toml qml6-module-qtquick-controls qt6-declarative-dev qt6-multimedia-dev  qml6-module-qtquick3d-spatialaudio
 ```

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.

# Additional steps for Ubuntu 22.04 LTS & 24.04 LTS

The available system version of Cython is too old in Ubuntu 22.04 & 24.04. You have to get the correct version
from pip:

```bash
pip3 install cython --break-system-packages
```

Please note that project requires at least **Cython 3.0.10**.

# Linux Mint Issue
Linux Mint has a [problem with `toml11`](https://github.com/SFTtech/openage/issues/1601), since CMake can't find it. To solve this, download the [toml11.zip](https://github.com/SFTtech/openage/files/13401192/toml11.zip), after, put the files in the `/usr/lib/x86_64-linux-gnu/cmake/toml11` path. (if the `toml11` directory doesn't exist, create it)
