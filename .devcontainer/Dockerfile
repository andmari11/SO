FROM debian:11.7

RUN apt-get clean
RUN apt-get update --fix-missing
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y locales sudo build-essential nano gcc make gdb glibc-doc git \
					man bash-completion whois strace time wget curl vim emacs manpages-dev procps htop 

# LOCALE
RUN sed -i '/es_ES.UTF-8/s/^# //g' /etc/locale.gen && locale-gen
ENV LANG es_ES.UTF-8
ENV LANGUAGE es_ES:es
ENV LC_ALL es_ES.UTF-8

# USUARIO
## Pasword hash: mkpasswd -m sha512crypt
RUN useradd -m usuarioso -p '$6$XwUTYzO/RnFcC8z0$Buha1JXlkbj2H0fkTLGQItXPWmtwA9qCrVIDIFakpEs2p4j0x6SdcZDTEh4u20kAFyFY14sf.vRuG44iEuKTm.' -s /bin/bash
RUN usermod -aG sudo usuarioso

USER usuarioso
WORKDIR /home/usuarioso

