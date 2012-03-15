#!/bin/sh

function get_package() {
	URL=$1
	TAR=$(basename $URL)
	if [ ! -f "$TAR" ]; then
		wget -O "$TAR" "$URL"
	fi
	echo "Unpacking $TAR"
	tar -xzf $TAR
}

get_package http://www.lua.org/ftp/lua-5.1.5.tar.gz
get_package http://www.kyne.com.au/~mark/software/download/lua-cjson-2.1.0.tar.gz

