#!/bin/sh

srcdir="`dirname $0`"
test -z "$srcdir" && srcdir=.

#export ACLOCAL="aclocal -I /usr/local/share/aclocal"
libtoolize --copy --force
aclocal
autoheader
automake -a -c
autoconf
#autoreconf -fvi

if [ "$1" = "--noconfigure" ]; then 
    exit 0;
fi

if [ X"$@" = X  -a "X`uname -s`" = "XSunOS" ]; then
    CC=/opt/SUNWspro/bin/cc CXX=/opt/SUNWspro/bin/CC $srcdir/configure "$@"
else
    $srcdir/configure "$@"
fi

