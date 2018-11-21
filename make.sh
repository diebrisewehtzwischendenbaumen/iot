#!/bin/bash

gcc -c cJSON.c
gcc -c server.c -lsqlite3 -lcurl
gcc cJSON.o server.o -o server -lsqlite3 -lcurl
