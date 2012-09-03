#!/bin/sh

# Packages the nexes, nmf js, readme, and license into a zip

OUT_DIR="tmp/aroma"
TMP_DIR=$(dirname $OUT_DIR)

VERSION=0.0.3

mkdir -p $OUT_DIR

make 64
cp *.nexe $OUT_DIR

make 32
cp *.nexe $OUT_DIR

cp *.nmf $OUT_DIR

mkdir $OUT_DIR/js
cp js/*.js $OUT_DIR/js

cp ../README.md $OUT_DIR/README.txt
cp ../LICENSE.md $OUT_DIR/LICENSE.txt

(
	cd $TMP_DIR
	apack "aroma-$VERSION.zip" aroma/
	mv *.zip ../
)

rm -rf $TMP_DIR

