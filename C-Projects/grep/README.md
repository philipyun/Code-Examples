# Search: A Parallel Grep

C implementation of a small subset of the Unix Command Grep

## Compile:

`gcc -std=c99 -o search search.c`

## Usage:

`search pattern [files...]`

## About:

Uses Unix pthreads to search multiple files against a pattern in parallel.
