#!/bin/sh

################################################################################
# Tests to make sure all ports directories have a corresponding PackageKit
# category mapped, using getcat.
#
# Usage: ./test
#     (You may need to make/install any programs listed below)

################################################################################
# Programs used in this test (relative paths will require make-ing, names
# imply external program installation).

CMD_GETCAT_DIR=../../backend/bin/getcat/
CMD_GETCAT="${CMD_GETCAT_DIR}pkgpk-getcat"

( cd "${CMD_GETCAT_DIR}"; make )
if [ $? -ne 0 ]
then
	echo "couldn't make getcat"
	exit 2
fi

# Quick and dirty way of getting all ports directories with the / required for
# getcat to work; assume all required ports directories are lowercase and all
# other directories have at least one uppercase character.
PORTS_DIRS=`ls -1F /usr/ports | grep '/$' | grep -v ".*[A-Z].*"`

# Test to make sure getcat retrieves "unknown" for silly ports dirs
GROUP=`${CMD_GETCAT} wombatwombatwombat`
if [ "${GROUP}" != "unknown" ]
then
	echo "test failed, did not return unknown for bad ports dir"
	exit 1
fi

for DIR in ${PORTS_DIRS}
do
	GROUP=`${CMD_GETCAT} ${DIR}`
	if [ -n "${GROUP}" ] && [ "${GROUP}" != "unknown" ]
	then
		echo "${DIR} -> ${GROUP}"
	else
		echo "test failed on ${DIR}"
		exit 1
	fi
done
