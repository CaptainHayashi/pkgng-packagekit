#!/bin/sh
# Functional test for installing/removing based on test repositories.

# Expect testpkg not to be installed initially
if [ -f /usr/local/testpkgs/testfile ]
then
	echo "Please remove testpkg and try again."
	exit
fi

# It should come up as available...
RESOLVE=`pkcon resolve --filter "~installed" testpkg | grep "Available.*"`
if [ -z "${RESOLVE}" ]
then
	echo "Test failed: testpkg is not showing as available."
	exit
fi

# ...but not installed.
RESOLVE=`pkcon resolve --filter "installed" testpkg | grep "Installed.*"`
if [ -n "${RESOLVE}" ]
then
	echo "Test failed: testpkg is showing as installed."
	exit
fi

# ToDo: put more tests here?

pkcon install testpkg

# Expect testpkg to now be installed
if [ ! -f /usr/local/testpkgs/testfile ]
then
	echo "Test failed: testpkg was not installed."
	exit
fi

# Now check to see if the correct files come up on a file list
pkcon get-files testpkg | awk -f strip_messages.awk > files
DIFF=`diff -rupN - files << EOF
Package files
  /usr/local/testpkgs/testfile
  /usr/local/testpkgs/testfile2
EOF
`
if [ -n "${DIFF}" ]
then
	echo "Test failed: file listing invalid, diff follows:"
	echo "${DIFF}"
	exit
fi

# It should come up as available...
RESOLVE=`pkcon resolve --filter "~installed" testpkg | grep "Available.*"`
if [ -n "${RESOLVE}" ]
then
	echo "Test failed: testpkg is showing as available post-install."
	exit
fi

# ...but not installed.
RESOLVE=`pkcon resolve --filter "installed" testpkg | grep "Installed.*"`
if [ -z "${RESOLVE}" ]
then
	echo "Test failed: testpkg is not showing as installed post-install."
	exit
fi

pkcon remove testpkg

# Expect testpkg to now be uninstalled again
if [ -f /usr/local/testpkgs/testfile ]
then
	echo "Test failed: testpkg was not uninstalled."
	exit
fi

# It should once more come up as available...
RESOLVE=`pkcon resolve --filter "~installed" testpkg | grep "Available.*"`
if [ -z "${RESOLVE}" ]
then
	echo "Test failed: testpkg is not showing as available post-remove."
	exit
fi

# ...but not installed.
RESOLVE=`pkcon resolve --filter "installed" testpkg | grep "Installed.*"`
if [ -n "${RESOLVE}" ]
then
	echo "Test failed: testpkg is showing as installed post-remove."
	exit
fi

echo "Test succeeded."
