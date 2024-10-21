#!/bin/bash

installpath="/usr/local/include"
libpath="/usr/local/lib"

cd ..
shell/bbt_copy_header_dir.sh $installpath bbt cxxlua

# sudo cp build/lib/libbbt_cxxlua.so /usr/local/lib/

# if [ ! -d "build" ];then
#     mkdir build
# fi