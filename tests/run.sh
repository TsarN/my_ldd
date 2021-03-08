#!/bin/bash

set -e

pushd "$(dirname "$0")" >/dev/null

mkdir -p build
cd build

cmake ../..
make my_ldd

MY_LDD="$PWD/my_ldd"

cd ..

for t in generated_*; do
    echo "$t"
    cd "$t"

    rm -rf build
    mkdir build
    cd build

    cmake .. >/dev/null 2>/dev/null
    make >/dev/null 2>/dev/null

    for exe in exe_*; do
        "$MY_LDD" "$exe" -p . -s > "../${exe}.out"
        grep -q "ld-linux-x86-64\\.so\\.2" "../${exe}.out"
        grep -v "ld-linux-x86-64\\.so\\.2" "../${exe}.out" > "../${exe}.out.2"
        diff "../${exe}.ans" "../${exe}.out.2"
    done

    cd ..
    cd ..
done

popd >/dev/null
