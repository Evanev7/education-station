#!/usr/bin/env bash
gcc main.c -Werror -Wpedantic -std=c11 -lraylib -lGL -o app
./app
