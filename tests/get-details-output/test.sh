#!/bin/sh

# Main test runner for the GetDetails output test.

while read LINE
do
	./test-pkg.sh ${LINE}
	if [ $? -ne 0 ]
	then
		echo "tests failed"
		exit 1
	fi
done < "test-pkgs"
