#!/bin/bash
g++ -std=c++20 -Wall -Wextra -pedantic -g -fsanitize=address,undefined "$1" -o "${1%.cpp}"

