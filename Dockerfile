FROM ubuntu:18.04

RUN apt-get update && apt-get install \
    cmake \
    cython3 \
    flex \
    git \
    libeigen3-dev \
    libepoxy-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    libharfbuzz-dev \
    libogg-dev \
    libopus-dev \
    libopusfile-dev \
    libpng-dev \
    libsdl2-dev \
    libsdl2-image-dev \
    make \
    python3-dev \
    python3-jinja2 \
    python3-numpy \
    python3-pil \
    python3-pip \
    python3-pygments \
    qml-module-qtquick-controls \
    qtdeclarative5-dev \
    -y && apt-get clean

WORKDIR /tmp
ADD . .
RUN ./configure --download-nyan && make
