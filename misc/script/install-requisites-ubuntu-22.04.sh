#!/bin/sh

sudo apt-get -y install \
beep \
gedit \
zbar-tools

LOGNAME=$(logname)

echo "$LOGNAME    ALL=NOPASSWD: /sbin/modprobe" >> /etc/sudoers
echo "$LOGNAME    ALL=NOPASSWD: /sbin/ip" >> /etc/sudoers
echo "$LOGNAME    ALL=NOPASSWD: /sbin/rmmod" >> /etc/sudoers
echo "$LOGNAME    ALL=NOPASSWD: /sbin/tc" >> /etc/sudoers