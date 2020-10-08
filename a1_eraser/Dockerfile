FROM ubuntu:18.04

# install dependencies
RUN apt-get update
RUN apt-get install -y git
RUN apt-get install -y make
RUN apt-get install -y gcc g++
RUN apt-get install -y check
RUN apt-get install -y cmake
RUN apt-get install -y valgrind
RUN apt-get install -y libunwind-dev
RUN apt-get install -y libdw-dev
RUN apt-get install -y liblzma-dev
RUN apt-get install -y binutils
RUN apt-get install -y libpthread-stubs0-dev
RUN apt-get install -y pkg-config
RUN apt-get install -y python3
RUN useradd eraser_user

ENV LANG C.UTF-8
ENV CMAKE_C_COMPILER /usr/bin/cc
ENV CMAKE_CXX_COMPILER /usr/bin/c++

USER eraser_user

# Define working directory.
WORKDIR /a1_eraser

# Define default command.
CMD ["bash"]
