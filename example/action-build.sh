#!/bin/bash

declare -i res

. ${IDF_PATH}/export.sh

cd /github/workspace/ESP32SimpleOTA/example

idf.py build
res=$?

echo "::set-output name=result::$res"
