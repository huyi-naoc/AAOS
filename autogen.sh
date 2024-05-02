#!/bin/bash

AUTORECONF=`which autoreconf 2>/dev/null`
AUTOMAKE=`which automake 2>/dev/null`
if [ $? -ne 0 ]
then
    echo "No 'autoreconf' found. You must install the autoconf package."
    exit 1
fi

$AUTORECONF -i
$AUTOMAKE --add-missing

echo
echo "----------------------------------------------------------------"
echo "Initialized build system. For a common configuration please run:"
echo "----------------------------------------------------------------"
echo
echo "./configure"
echo
