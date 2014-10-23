#!/bin/sh
# Functional test for GetDetails based on test repositories.

pkcon get-details testpkg | awk -f strip_messages.awk > testmsg

DIFF=`echo "${SUCCESS_MSG}" | diff -rupN - testmsg << EOF
Package description
  package:     testpkg-1.0.0.freebsd:9:x86:32
  license:     BSD
  group:       multimedia
  description: This is a test package for the PackageKit backend.

WWW: http://wiki.freebsd.org/SummerOfCode2013/pkgPackagekit
  size:        123456789 bytes
  url:         https//wiki.freebsd.org/SummerOfCode2013/pkgPackagekit
EOF
`

# Expect the diff to be empty.
if [ -n "${DIFF}" ]
then
	echo "Test failed; diff follows:"
	echo "${DIFF}"
	exit
fi
echo "Test succeeded."
