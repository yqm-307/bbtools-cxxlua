#!/bin/bash

installpath="/usr/local/include"
libpath="/usr/local/lib"

cd ..
shell/bbt_copy_header_dir.sh $installpath bbt cxxlua

sudo cp build/lib/libbbt_cxxlua.a /usr/local/lib/
