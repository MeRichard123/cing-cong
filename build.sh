#!/bin/sh

set -xe

clang -Wall -o pong pong.c -lraylib `pkg-config --libs --cflags raylib`
