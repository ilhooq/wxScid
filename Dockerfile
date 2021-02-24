FROM ubuntu:20.04

RUN apt-get update && apt-get -y install \
    g++ make automake autoconf libtool libwxgtk3.0-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* /usr/share/doc/* \
    && mkdir /project

WORKDIR /project
