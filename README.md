# Data Parser

This is an educational/practice project. 

Read and write structured text data in JSON and eventually maybe others.

## Build And Run

```sh
make run JSON='"Hello, World!"'
```

```sh
make
./echo '"Hello, World"'
```

## Test

Tests require Perl and _should_ work with any modern Linux OS's system Perl installation.

Test files are stored in the `t` directory.

```sh
prove
```

Or

```sh
prove -v
```

## Memory Testing

Run with Valgrind, useful for debugging segfaults and memory leaks:

```sh
make debug
```
