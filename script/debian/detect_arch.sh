#!/bin/bash

arch=$(dpkg --print-architecture 2>/dev/null)

if [[ "$1" != "force" ]]; then
    if [[ "$arch" != "amd64" && "$arch" != "arm64" ]]; then
        arch="unsupported"
    fi
fi

echo $arch
