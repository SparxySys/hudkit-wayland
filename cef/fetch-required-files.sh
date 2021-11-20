#!/bin/bash

CEF_BUILD='cef_binary_95.7.18+g0d6005e+chromium-95.0.4638.69_linux64_minimal'
CEF_BUILD_ESCAPED="$(echo "$CEF_BUILD" | sed -e 's/+/%2B/g')"

CEF_URL="https://cef-builds.spotifycdn.com/${CEF_BUILD_ESCAPED}.tar.bz2"

if [[ ! -f "v8_context_snapshot.bin" ]]; then
    mkdir 'cef-project' &> /dev/null
    echo 'Fetching required CEF binaries'
    curl -ocef-project/tmp.tar.bz2 "${CEF_URL}" &> /dev/null
    pushd cef-project > /dev/null
    echo 'Extracting and moving'
    tar xf tmp.tar.bz2
    mv cef_binary_*/Release/* ../
    mv cef_binary_*/Resources/* ../
    echo 'Cleaning up'
    popd > /dev/null
    rm -rf cef-project
fi
