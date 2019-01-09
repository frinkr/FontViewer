#!/bin/bash
CURRENT=$PWD
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$(uname)" == "Darwin" ]; then
    BUILD=build
else
    BUILD=../fvbuild
fi

[ "$1" = "clean" ] && rm -rf "$DIR/$BUILD"
[ ! -d "$DIR/$BUILD" ] &&  mkdir -p "$DIR/$BUILD"

cd "$DIR/$BUILD"

if [ "$(uname)" == "Darwin" ]; then
    cmake -G Xcode .. -DCMAKE_PREFIX_PATH="/usr/local/Cellar/qt/5.11.2/"
else
    cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_VERSION=4.7 ../FontViewer
fi

cd "$CURRENT"
