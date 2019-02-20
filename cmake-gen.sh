#!/usr/bin/env bash

CURRENT=$PWD
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

BUILD=build

[ "$1" = "clean" ] && rm -rf "$DIR/$BUILD"
[ ! -d "$DIR/$BUILD" ] &&  mkdir -p "$DIR/$BUILD"

cd "$DIR/$BUILD"

if [ "$(uname)" == "Darwin" ]; then
    export PKG_CONFIG_PATH="/usr/local/opt/openssl/lib/pkgconfig" # required by Podofo
    cmake -G Xcode .. -DCMAKE_PREFIX_PATH=`brew --prefix qt`
else
    cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_VERSION=4.7 ../FontViewer
fi

cd "$CURRENT"
