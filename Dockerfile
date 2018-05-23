FROM ubuntu:18.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt update \
    && apt -y upgrade \
    && apt -y install build-essential gcc-multilib g++-multilib binutils-multiarch \
    && apt -y install gdb strace ltrace netcat vim less wget git socat sudo byobu sshpass \
    && apt -y install python-pip python-dev \
    && apt clean

RUN git clone https://github.com/pwndbg/pwndbg \
    && cd pwndbg \
    && ./setup.sh \
    && cp /root/.gdbinit /etc/gdb/gdbinit \
    && apt clean

RUN pip install virtualenv
