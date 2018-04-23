#!/bin/bash
CURRENT=$PWD
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

[ "$1" = "clean" ] && rm -rf "$DIR/build"
[ ! -d "$DIR/build" ] &&  mkdir -p "$DIR/build"

cd "$DIR/build"

if [ "$(uname)" == "Darwin" ]; then
    cmake -G Xcode .. -DCMAKE_PREFIX_PATH="/usr/local/Cellar/qt/5.10.0/"
else
    cmake -G "Eclipse CDT4 - Unix Makefiles" ..
fi

cd "$CURRENT"
