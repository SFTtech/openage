# Prerequisite steps for Ubuntu users (Ubuntu >= 23.04)

Run the following commands:

 - `sudo apt-get update`
 - `sudo apt-get install g++ cmake cython3 libeigen3-dev libepoxy-dev libfontconfig1-dev libfreetype-dev libharfbuzz-dev libogg-dev libopus-dev libopusfile-dev libpng-dev libsdl2-dev libsdl2-image-dev libtoml11-dev python3-dev python3-mako python3-numpy python3-lz4 python3-pil python3-pip python3-pygments python3-toml qml6-module-qtquick-controls qt6-declarative-dev qt6-multimedia-dev  qml6-module-qtquick3d-spatialaudio`

You will also need [nyan](https://github.com/SFTtech/nyan/blob/master/doc/building.md) and its dependencies.

# Additional steps for Ubuntu 22.04 LTS

The available system version of Cython is too old in Ubuntu 22.04. You have to get the correct version
from pip:

```
pip3 install cython --break-system-packages
```
