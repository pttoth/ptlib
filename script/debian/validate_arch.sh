#!/bin/bash

arch=$1

if [[ "$arch" == "amd64" ]]; then
    exit 0
fi

exit 1
