#!/bin/sh

LUA_SRC=http://www.lua.org/ftp/lua-5.1.5.tar.gz
LUA_TAR=$(basename $LUA_SRC)

if [ ! -f "$LUA_TAR" ]; then
	wget -O $LUA_TAR http://www.lua.org/ftp/lua-5.1.5.tar.gz
fi

tar -xzf $LUA_TAR

# rm $LUA_TAR
