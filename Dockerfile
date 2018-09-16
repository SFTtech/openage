FROM ubuntu:18.04

RUN apt-get update && apt-get -y install cmake cython3 libeigen3-dev libepoxy-dev libfontconfig1-dev libfreetype6-dev libharfbuzz-dev libogg-dev libopus-dev libopusfile-dev libpng-dev libsdl2-dev libsdl2-image-dev python3-dev python3-jinja2 python3-numpy python3-pil python3-pip python3-pygments qml-module-qtquick-controls qtdeclarative5-dev && apt-get clean

RUN apt-get update && apt-get -y install git && apt-get clean

RUN apt-get update && apt-get -y install cmake flex make && apt-get clean

ADD . /tmp
WORKDIR /tmp
RUN ./configure --download-nyan && make
