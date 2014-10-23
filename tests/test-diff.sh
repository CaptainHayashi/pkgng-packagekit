#!/bin/sh
#
# Performs a diff between two files that represent the PkgNG and PackageKit
# outputs after performing analogous commands, modified to follow the format
# of one or the other.  The two files should be identical for the test to
# succeed; if it fails then usually the PackageKit backend is reporting
# incorrect data.
#
# Usage: test-diff.sh pkg-file pkcon-file

DIFF="`diff -rupN \"$1\" \"$2\"`"
if [ -n "${DIFF}" ]
then
        echo "test failed with diff:"
        echo "${DIFF}"
        RETURN=1
else
        echo "test succeeded"
        RETURN=0
fi

