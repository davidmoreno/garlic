#!/bin/sh

echo "This is a simple test. Actually random"
echo "Program arguments <$*>"
echo "Extra ENVVARS"

export | grep GARLIC

echo " -- "

RND=$( od -d -N1 -An /dev/urandom )
echo $RND

sleep 3

if [ "$(( $RND > 64 ))" = 1 ]; then
	echo "Error!!!"
	exit 1
else
	echo "Everything ok"
	exit 0
fi
