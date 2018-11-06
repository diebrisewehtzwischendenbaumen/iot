#!/bin/bash

gcc -c cJSON.c
gcc -c testJSON.c
gcc cJSON.o testJSON.o -o test
