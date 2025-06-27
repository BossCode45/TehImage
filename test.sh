#!/usr/bin/env bash

make test && LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH ./image_test $@
