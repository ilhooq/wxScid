FROM ubuntu:18.04

RUN apt-get update && apt-get -y install g++ make automake autoconf libtool libwxgtk3.0-dev

RUN useradd ilhooq

WORKDIR /home/ilhooq/wxScid

USER ilhooq
