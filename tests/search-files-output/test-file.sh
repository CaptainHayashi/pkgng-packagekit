#!/bin/sh

################################################################################
# Test the output of `pkcon search file` to see if it agrees with `pkg which.`
# Expects one argument, the file which shall be passed to both.

# Output files
PKG_OUT=out.pkg
PKCON_OUT=out.pkcon

# Get the expected ABI through some invasive inspection of the pkg config dump
ARCH="`pkg -vv | grep ABI | sed 's/ *ABI: *//'`"

echo "Looking for provider of $1..."
pkg which -q "$1" |
	sed "s/^/Installed    /; s/$/.${ARCH}/" > ${PKG_OUT}

pkcon search file "$1" |
	awk -f "../strip_messages.awk" > ${PKCON_OUT}

../test-diff.sh ${PKG_OUT} ${PKCON_OUT}
return $?
