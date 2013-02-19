#!/bin/sh

echo "This is a simple test. Actually random"

sleep 3

RND=$( od -d -N1 -An /dev/urandom )
if [ "$(( $RND > 127 ))" = 1 ]; then
	echo "Error!!!"
	exit 1
else
	echo "Everything ok"
	exit 0
fi
