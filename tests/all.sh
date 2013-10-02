#!/bin/sh

[ -d "tests" -a -d "src" ] || ( echo "Run this test script from the base dir of garlic" ; exit 1)

for t in $( ls tests/[0-9][0-9]-*.sh ); do
	echo "Running $t"
  ./$t
  if [ "$?" != "0" ]; then
		echo "Error on $t"
		exit 1
	fi
done

exit 0


