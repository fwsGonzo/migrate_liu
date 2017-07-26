#!/bin/bash

cat < /dev/tcp/10.0.0.40/1337 > disk/state.file
boot . -c
