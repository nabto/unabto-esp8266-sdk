#!/bin/bash
# This script packs all relevant files into a zip library.

NAME=unabto-esp8266-sdk

mkdir -p $NAME

cp -r src $NAME/
cp -r examples $NAME/
cp keywords.txt $NAME/
cp library.properties $NAME/

zip -rq $NAME.zip $NAME

rm -rf $NAME

echo "Library $NAME.zip created!"
