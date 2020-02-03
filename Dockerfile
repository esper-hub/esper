FROM debian:stretch-slim

RUN apt-get update && apt-get install -y \
	make \
	wget \
	gcc g++ \
	autoconf automake \
	flex bison \
	git \
	unzip bzip2 \
	libtool libtool-bin \
	gperf \
	texinfo help2man \
	gawk sed \
	ncurses-dev \ 
	libexpat-dev \
	python python-dev python-serial \
	&& rm -rf /var/lib/apt/lists/*

# Create user
RUN useradd -m builder
USER builder

## Install ESP8266 SDK - this is only needed for the toolchain
RUN git clone https://github.com/pfalcon/esp-open-sdk.git /home/builder/esp-open-sdk \
    && cd /home/builder/esp-open-sdk \
    && git reset --hard c70543e57fb18e5be0315aa217bca27d0e26d23d \
    && git submodule update --init --recursive \
    && cd /home/builder/esp-open-sdk; make VENDOR_SDK=1.5.4 STANDALONE=y \
    && rm -rf /home/builder/esp-open-sdk/crosstool-NG/.build

ENV ESP_HOME /home/builder/esp-open-sdk
ENV SMING_HOME /home/builder/Sming/Sming

# Install sming
RUN git clone https://github.com/esper-hub/Sming.git /home/builder/Sming \
    && cd /home/builder/Sming \
    && git reset --hard f808b88ee28092bcda2b4421aec7862b7aebb442 \
    && git submodule update --init --recursive \
    && cd /home/builder/Sming/Sming \
    && make clean \
    && make

ENV COM_PORT /dev/ttyESP
ENV COM_SPEED 115200

# Copy local esper
COPY . /home/builder/esper/

# Switch back user and set workdir for building
USER root
WORKDIR /home/builder/esper

# Set variables for build
ENV ESPER /home/builder/esper
ENV SITE /home/builder/site

CMD make

