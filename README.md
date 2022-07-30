# LC-3 Assembler

This project is an assembler written in C for [Little Computer 3 (LC-3)]((https://en.wikipedia.org/wiki/Little_Computer_3))


## LC-3 Instruction Set Architecture (ISA)

### Memory organisation
The LC-3 memory has an address space of 2^16 (65536) locations, and an addressability of 16 bits.

The normal unit of data that is processed in the LC-3 is 16 bits, we refer to 16 bits as one word, and we say the LC-3 is word-addressable.

### Registers
The LC-3 specifies eight general purpose registers, each identified by a 3-bit register number. They are referred to as RO, R1 ... R7.

Registers are used as memory locations to store information. The number of bits stored in each register is 16 (one word).

Registers can be accessed in a single machine cycle as opposed to data from memory that normally requires more than one cycle.

### Instruction set
An instruction is made up of two things: opcode and operands.

The instruction set of an ISA is defined by its set of opcodes, data types, and addressing modes. The addressing modes determine where the operands are located.

The LC-3 ISA has 15 instructions, each identified by its unique opcode. The opcode is specified by bits [15:12] of the instruction. Since four bits are used to specify the opcode, 16 distinct opcodes are possible. However, the LC-3 ISA specifies only 15 opcodes. The code 1101 has been left unspecified, reserved for some future need.

There are three different types of instructions, which means three different types of opcodes: 

- operates instructions: process information 
- data movement instructions: move information between memory and the registers and between registers/memory and input/output devices 
- control instructions: change the sequence of instructions that will be executed (instead of processing them sequentially according to their location in memory)
    - conditional branch
    - unconditional jump
    - subroutine (function) call
    - TRAP (system calls, PC changes to a memory address that is part of the operating system so that the operating system will perform some task on behalf of the program)
    - return from interrupt

### Data types
The data type of the operands is 16-bit 2's complement integers.

### Addressing modes
An addressing mode is a mechanism for specifying where the operand is located. For instance, a 16-bit integer does not fit in an instruction, therefore the only way an opcode can operate on said integer is by storing it in memory/register and use as operand a reference to that location.

An operand can generally be found in one of three places:

- in memory, 
- in a register, or 
- as a part of the instruction (in this case, the operand is called literal or immediate)

The LC-3 supports five addressing modes:

- __immediate__ (or literal)
- __register__
- memory addressing modes:
    - __PC-relative__: bits [8:0] of the instruction specify an offset relative to the PC. The memory address is computed by sign- extending bits [8:0] to 16 bits, and adding the result to the incremented PC
    - __indirect__: in this case, bits [8:0] do not contain the operand but the memory addres of the operand
    - __Base+offset__: bits [5:0] of the instruction specify an offset relative to a base register. The memory address is computed by sign- extending bits [5:0] to 16 bits, and adding the result to the base register.

### Condition codes
Condition codes allow the instruction sequencing to change on the basis of a previously generated result.

The LC-3 has three single-bit registers (condition codes) that are set (set to 1) or cleared (set to 0) each time one of the eight general purpose registers is written. The three single-bit registers are called N (negative), Z (zero) and P (positive).


## LC-3 Assembly language

This is the specification of the assembly language corresponding to the previously described ISA.

The LC-3 assembler is the program that takes as input a computer program written in LC-3 assembly language and translates it into a program in the ISA of the LC-3.

### Instructions

(LABEL) OPCODE OPERANDS (; COMMENTS)

- The OPCODE is a symbolic name for the opcode of the corresponding LC-3 instruction.
- The number of OPERANDS depends on the operation being performed and are separated by commas
- Labels are symbolic names that are used to identify memory locations that are referred to explicitly in the program
- Comments are identified by semicolons and are ignored by the assembler

Labels and comments can also appear on their own line (without accompanying any instruction).
Labels always make reference to the memory location of the first instruction after the label.
Two consecutive labels on the same line is considered illegal. However, two consecutive labels
on different lines is permitted.

### Pseoud-ops (assembler directives)
An assembler directive is a message to help the assembler in the assembly process. Once the assembler handles the message, the pseudo-op is discarded.

- __.ORIG__: tells the assembler where in memory to place the LC-3 program
- __.FILL__: tells the assembler to set aside the next location in the program and initialize it with the value of the operand.
- __.BLKW__: tells the assembler to set aside some number of sequential memory locations (BLocK of Words) in the program
- __.STRINGZ__: tells the assembler to initialize a sequence of n + 1 memory locations; the argument is a sequence of n characters, inside double quotation marks; the  first n words of memory are initialized with the zero-extended ASCII codes of the corresponding characters in the string; the final word of memory is initialized to 0.
- __.END__: tells the assembler where the program ends; any characters that come after .END are ignored by the assembler.


Here's the list of resources taken as reference for the implementation

- [GUI simulator](https://www.cis.upenn.edu/~milom/cse240-Fall05/handouts/lc3guide.html)

- [simulator (Virtual Machine) implementation](https://justinmeiners.github.io/lc3-vm/)

- [learning resources](https://highered.mheducation.com/sites/0072467509/student_view0/index.html)

![learning resources](./images/student_resources.png)


- [LC-3 details](https://people.cs.georgetown.edu/~squier/Teaching/HardwareFundamentals/LC3-trunk/docs/)

![learning resources](./images/lc3_resources.png)


## Implementation notes

Each line of the assembly file corresponding to an instruction is converted into a 16-bit integer (using __big-endian__ format).

Strictly speaking, the assembler does not need to know where the program will be loaded in memory as it only deals with offsets.

It's enough for the assembler to write the operand of .ORIG as the first instruction of the object file so that the LC-3 virtual machine knows where to load the program (the memory address in which an instruction is stored has a direct correspondence to the position of said instruction inside the assembly file (after discarding labels, comments and directives): the position is the offset to be added to the value defined by the .ORIG directive.)

Having said that, in order to generate a symbol table useful for debugging, it is desirable for the assembler to take into account the value given by .ORIG when generating the symbol table.

It's worth noting that instructions and data shared the same address space and therefore can be intermingled in memory.

The number of memory cells is 2**16, therefore a memory address fits in a memory cell
(making it possible to have pointers)


## Build

To build the project and run the tests, either one of the following options can be used:

- VSCode tasks

- Makefile

VSCode tasks are specially useful to debug the application.


For details about the different compilation flags, see the [GNU docs](https://gcc.gnu.org/onlinedocs/gcc-10.1.0/gcc/Invoking-GCC.html#Invoking-GCC). 

Here's a brief description of some of the flags used in this project.

#### -fno-common

Used to disable [common symbols generation](https://stackoverflow.com/questions/66044467/why-does-global-variable-definition-in-c-header-file-work).

The justification to use this flag is to enforce good practices when defining global variables, namely: define them in exactly one .c file, then declare them as an extern in a header file which then gets included in the required .c files

#### address sanitizer

https://developers.redhat.com/blog/2021/05/05/memory-error-checking-in-c-and-c-comparing-sanitizers-and-valgrind

## Unit tests

Unit tests are executed with the command:

`make unittest`

After running the unit tests, a browser window will open with the test coverage information, that is the final product of using [gcov](https://gcc.gnu.org/onlinedocs/gcc-10.1.0/gcc/Gcov-Intro.html#Gcov-Intro) and [lcov](http://ltp.sourceforge.net/coverage/lcov.php). See next section for more details.

https://docs.gtk.org/glib/testing.html

https://www.manpagez.com/html/glib/glib-2.42.2/glib-Testing.php

### Test coverage

Test coverage is done with [gcov](https://gcc.gnu.org/onlinedocs/gcc-10.1.0/gcc/Gcov-Intro.html#Gcov-Intro), that takes as input two files, [.gcda and .gcno](https://gcc.gnu.org/onlinedocs/gcc-10.1.0/gcc/Gcov-Data-Files.html#Gcov-Data-Files), which are generated thanks to the compilation flags `-fprofile-arcs` and `-ftest-coverage option`, respectively.

The output of gcov is  *.gcov files containing the profiling information in a human-readable format. That information can be better displayed with a graphical tool like [lcov](http://ltp.sourceforge.net/coverage/lcov.php)

__Note__: in MacOS, lcov can be installed with `brew install lcov`


## Acceptance tests

Acceptance tests are run using a test oracle to compare the oracle predictions to the results of the application.

The oracle is the assembler utility included in:

```
wget https://acg.cis.upenn.edu/milom/cse240-Fall05/handouts/code/LC3sim.jar                                                            
```

`LC3sim.jar` is also a runtime environment to execute the binary code generated by
the assembler.

To execute `LC3sim.jar`, run

```
java -jar LC3sim.jar &
```

## Application

`make myfile CPPFLAGS=-DFAB_MAIN`



## Support tools

The folder `tools` contains some utilities to aid with the development of this assembler:

* `lc3objdump` is a version of [objdump](https://en.wikipedia.org/wiki/Objdump) to print the binary content of an object file generated by the LC3 assembler; Makefile shows how to run it

__STDC_VERSION__
macro whose value indicates which standard is supported by the compiler

## Shared objects

cmocka for unit testing
brew install cmocka

another library for unit testing
glib2.0
sudo apt-get update
sudo apt-get install libglib2.0-dev


