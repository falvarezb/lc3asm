# C template
template for my C projects

## Components

- Main file: myfile.c

- Header: myfile.h

- Unit tests: myfile_test.c

The method `main()` in myfile.c is guarded by the macro `FAB_MAIN` so that it can be left out of the binary when running unit tests


## Builder

To build and run the project and the tests, there is availbble: 

- VSCode tasks

- Makefile

The following sections show how to use Makefile.

### Unit tests

`make unittest`

### Application

`make myfile CPPFLAGS=-DFAB_MAIN`

