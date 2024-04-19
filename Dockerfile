FROM ubuntu:20.04

WORKDIR /deps

ENV DEBIAN_FRONTEND noninteractive

RUN apt update && apt install -y \
    build-essential \
    wget \
    cmake \
    git \
    pkg-config \
    python3 \
    libfmt-dev \
    libssl-dev \
    libcapstone-dev \
    libfuse2 \
    appstream \
    rapidjson-dev

RUN git clone https://github.com/keystone-engine/keystone.git && mkdir -p keystone/build && cd keystone/build && ../make-share.sh && make install && ldconfig

WORKDIR /patcher

COPY . .

RUN chmod +x start.sh