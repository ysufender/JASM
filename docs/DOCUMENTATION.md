
# JASM Documentation

## Introduction

JASM is a bytecode assembly backend for creating scripting languages that will compile to JASM bytecode. The
purpose of this project is to create a backend for easily creating scripting languages that'll be embedded into C++ code,
or will be used as a regular lanugage. With this way, anyone that wants to create a scripting language will be 
free of performance and execution based headaches, thus being able to focus on their language design and leave the rest
to JASM. In addition, the future plans for this project includes adding support for converting JASM and its bytecode to
GNU or LLVM ILs. Maybe a utopic plan for now, but who knows?

JASM is a part of CSLB project. The CSLB project (which stands for Common Scripting Language Backend, pronunced as SeezleBee)
consists of three parts: Assembler, Linker and Runtime. The assembler and linker parts are what this repo covers. JASM is
the name of the IL, bytecode, assembler and linker altogether. The runtime is CSR (Common Script Runtime, pronunced as Caesar, with proper latin pronunciation of C as K as in King, so Kaesar), and it has its own repo [Work in Progress].

## Installing JASM 

You can either grab the compiled binaries from the release section (if there exists such binaries) or build JASM from source.
See [BUILD.md](./BUILD.md) for building from source.

## Table of Contents

- [JASM Documentation](#jasm-documentation)
	- [Introduction](#introduction)
	- [Installing JASM](#installing-jasm)
	- [Table of Contents](#table-of-contents)
	- [The Project Structure](#the-project-structure)
        - [The CLI](#the-cli)
	    - [The Assembler](#the-assembler)
        - [The Linker](#the-linker)
    - [Regarding the Output Formats](#regarding-the-output-formats)
	    - [Executable (jef)](#executable)
	    - [Library (stc and shd)](#library)
	    - [Object File](#object-file)
    - [The JASM IL](#the-jasm-il)
        - [Sections](#sections)
            - [init](#init)
            - [prep](#prep)
                - [org](#org)
                - [sts](#sts)
                - [sth](#sth)
                - [imp](#imp)
            - [body](#body)
            - [end](#end)
        - [Registers](#registers)
        - [Syntax](#syntax)
	        - [Comments](#comments)
            - [Symbols](#symbols)
            - [Instructions](#instructions)
                - [Modeflags](#modeflags)
                - [List of Instructions](#list-of-instructions)
        - [Atomic Types](#atomic-types)
        - [Splitting Code Accross Files](#splitting-code-accross-files)
            - [Referencing Unknown Symbols](#referencing-unknown-symbols)
	- [Why This Documentation Does Not Explain How The IL Works?](#why-this-documentation-does-not-explain-how-the-il-works)
	
## The Project Structure

The JASM project includes the assembler(s) and the linker(s) for the JASM IL.

```
./
|_ asm/ ------------------------> contains the test.jasm file which I used to test the assembler. I left it there for funs.
|_ docs/ -----------------------> contains documentation `.md`s, concept `.jasm`s and various descriptive `.txt` files.
|_ include/
| |_ bytemode/
| | |_ assembler/
| | |_ linker/
| |_ extensions/
|_ lib/ ------------------------> contains third-party libraries
|_ src/
  |_ bytemode/ -----------------> the project is designed to allow future improvements and additions like assembler for assembling 
  | |_ assembler/                 to machine code instead of byte that's why the `assembler` and `linker` reside under `bytemode`
  | |_ linker/  
  |_ core/ ---------------------> the core functions that doesn't change depending on the target mode
  |_ extensions/ ---------------> various utility functions, like serialization of types to bytes.
```

### The CLI

The CLI is mostly handled by the [CLI Parser](https://github.com/ysufender/CLIParser), so if you want to know how it works you have to check its source.
As of the CLI usage, basic CLI usage is covered in [README.md](../README.md#basic-cli-usage). If you wan't to know more about the CLI parameters and flags, this section covers that.

#### silent

`jasm --silent` or `jasm -s` (with a lowercase `s`)

The logging system in the JASM project has four modes: `LOG`, `LOGD`, `LOGW` and `LOGE`.

The modes `LOG`, `LOGD` and `LOGW` are disabled when the `silent` flag is set. The `LOGE` is only active
when its in `Medium` or `High` mode. Any unhandled exception is non-silent no matter the `silent` flag.

#### single

`jasm --single` or `jasm -S` (with an uppercase `S`)

By default, JASM first assembles all the input files and then links them together and creates a final file depending on the 
`--lib-type` flag. However when `single` flag is set, JASM stops after the assembling process and ~~only creates "`jo (jasm object)`" 
files. Each of these object files follow the `static library` structure and can then later be used as static libraries.~~ creates static libraries "`stc`" from each file, which can later be linked to other libraries and executables.

#### out

`jasm --out <value>` or `jasm -o <value>` (with a lowercase `o`)

By default, JASM will use the first entry from the input files to generate an output name based on it and target output type.
The ouput will be placed in the same directory as the first input file. Optionally you can provide a name along with a path for
the output file. Be aware that if the name you've provided contains an extension that doesn't match with the target output type,
JASM will simply append the correct extension at the end of the name you've provided.

For example if you provide `out.jef` for a target static library, the output will be `out.jef.stc`.

#### lib-type

`jasm --lib-type <value>` (where `value` can be either `stc` or `shd`)

By default, JASM will assume that the target output format is executable unless you specify 
otherwise via `lib-type` flag. The flag accepts two possible values, one being `stc` for `STatiC`
and the other `shd` for `SHareD`. If a `lib-type` is provided but it is not `stc` or `shd`, JASM 
will proceed by assuming it as an executable.

#### in

`jasm --in <..values..>` or `jasm -I <..values..>`

Source files to assemble and (optionally) link. If the target output format is an executable, the first file must contain
the program entry point instruction `org` instruction along with `sts` and `sth`.

#### libs

`jasm --libs <..values..>` or `jasm -L <..values..>`

Libraries to be used and to be linked. ~~~Must contain the shared libraries as well if any used in the project.~~ Only libraries to be linked statically must be present, shared libraries are checked for at runtime.

#### pipelines

`jasm --pipelines` or `jasm -p`

By default, JASM will use intermediary object files. If the flag `pipelines` is set, pipeline streams are used instead.

> WARNING: Pipelines option is not currently available. If you try to use it, JASM will not complain, but will still use object files. You can check the availability of pipelines using `jasm --help`.

#### working-dir

`jasm --working-dir <value>` or `jasm -w`

By default, JASM will use the directory that it's been executed from as working directory unless you specify otherwise via
the `working-dir` flag. The value of the flag `working-dir` must be an existing directory. Otherwise an error will be thrown.

#### redirect-stdout

`jasm --redirect-stdout <..values..>` or `jasm -r <..values..>`

By default, JASM will use the stdout/stderr streams for logging unless you specify otherwise via `redirect-stdout` flag.
The flag accepts two values at max; if only one is given, it'll be used as both stdout and stderr; if two is given, they'll be
used as stdout and stderr respectively. If more that two values has been given, the program will silently log an error but
continue the process.

> WARNING: Currently there is an issue with stdout redirection that causes segmentation fault.

### The Assembler

Although not yet implemented, JASM is designed to allow modular assemblers. Every assembler lies within a `<identifier>mode`
directory under `src`. Currently only available assembler is `bytemode` assembler. It assembles the JASM IL into JASM bytecode which is meant to be executed by the `Common Script Runtime (CSR) [Work in Progress]`.

#### The Byte Assembler

The assembler follows the following procedure:

If target is an executable:

1. Assemble the first file in executable format. Store the AssemblyInfo of the result in memory.
2. Assemble the rest as object files and store their AssemblyInfo's in memory.

If target is a library:

1. Assemble every file as object files and store their AssemblyInfo's in memory.

If the single mode is active:

1. Assemble each file in library format. 

Regardless of the target, the result AssemblyInfo's are then passed to the linker.
The way the assembler works is quite straightforward, the `Stream::Tokenize` function under `extensions/streamextensions.cpp` creates a token from the input stream that is passed to it. I say token, but it just returns the next "word" from the stream. Only exception is that if there exists something between double-quotes, then it is returned as a single "word". For example both `main:` and `"simple assembler"` are a single token.
Then there are three ways the assembler can go:

1. The token is a label i.e. ends with a colon -> a symbol at current location is created
2. The token is present in the instruction map -> corresponding instruction function is called
3. The token marks the end of `.body` segment i.e. is `.end` -> assembler finishes assembling the current file.

If the token doesn't fall under these three categories, an exception occurs and program exits with an error.
Regardless of the target and the mode, the assembler creates intermediary object files from each file, and then passes the resulting AssemblyInfoCollection to linker, which contains the symbol informations and paths of said object files. The linker handles the rest from here on.

### The Linker

Just as the assembler, JASM is designed to allow modular linkers. Every linker lies within a `<identifier>mode`directory under `src`. Currently only available linker is `bytemode` linker. It links the object files created by the assembler and checks for symbol errors. The serialization of AssemblyInfo's are done by the linker, if the single mode is active or the target is a library.
The way the linker works is, again, quite simple. It accepts an AssemblyInfoCollection, such that the first AssemblyInfo is in the executable format if the target is an executable. The linker then iterates through each AssemblyInfo and merges them into a single file, that is if the single mode is not active. After the merging process, the linker checks if there are still unknown symbols present. 
If there are, then it checks whether the program will use any runtime assemblies or not. If not then the linker exits with an error, otherwise continues silently and finishes its job. Before finishing, it serializes the resulting AssemblyInfo at the end of the final file if symbol info flag is marked in the current AssemblyContext.
If the single mode is active, then instead of merging each object file into one final file, it serializes the AssemblyInfo's of each file at the end of them and creates static libraries from each one of them.
The linker does a brief check after every AssemblyInfo has been linked into the final output; it gets the AssemblyInfo's of every runtime assembly that the current AssemblyContext marks as "will be used" and checks if the unknown symbols (if there exists such thing after linking) exist within them. If not then it exits without an error, but prints a warning message.

> NOTE: Static libraries can have runtime assembly dependencies.

## Regarding the Output Formats

Below are the various file formats used by JASM. They are, for some reason, defined with a strange syntax I used when I first started this project. The syntax description can be found in [this file](./defines.txt).

### Executable

The executable format has two variants, one containing the AssemblyInfo and the other being just bare bytecode. Both are defined in a single style as it follows:

```
u32;    entry point;    entry point of the program decided by the org instruction
u32;    stack size;     stack size of the program decided by the sts instruction
u32;    max heap size;  maximum heap size of the  program decided by the sth instruction
???;    bytecode;       compiled bytecode. It's size can be maximum of which u32 can hold
_???;    AssemblyInfo;   serialized assembly info. It's size can vary from a few bytes to a couple hundred
_u64;    size;           size of the AssemblyInfo 
```

### Library

Both the static and runtime libraries have the same format, it is defined as it follows:

```
???;    bytecode;       compiled bytecode. It's size can be maximum of which u32 can hold   
???;    AssemblyInfo;   serialized assembly info. It's size can vary from 1 byte to a couple hundred    
u64;    size;           size of the AssemblyInfo 
```

### Object File

The object file format has two variants, one for the main executable file that contains the entry
point instruction, and one for the remaining files. They both defined in a single style as it 
follows:

```
if (library):
    ???;    bytecode;       compiled bytecode. It's size can be maximum of which u32 can hold      
else:
    u32;    entry point;    entry point of the program decided by the org instruction   
    u32;    stack size;     stack size of the program decided by the sts instruction    
    u32;    max heap size;  maximum heap size of the  program decided by the sth instruction    
    ???;    bytecode;       compiled bytecode. It's size can be maximum of which u32 
```

## The JASM IL

The JASM IL is the "assembly language" that is used in CSLB project. It is a basic language
that doesn't have cool features like `namespaces`, `functions`, `variables` or even `types`.
Well, something like types exist but not quite like what you might think. 

JASM IL uses `symbols` (which are generally called labels) for conditionals, functions
and compile-time constants. Although it utilizes a couple of registers for use, they are
primarily used for storing pointers, parameter sizes and a couple of flags. So JASM is
essentially a stack-based language.

The general structure of a JASM file is like this:

```
everything here is discarded unless it is ".prep" without quotes, which starts
the preparation section.

.prep
    #here lies the preparation instructions#
.body
    #here lies the actual code#
.end

everything here is discarded
```

Usually, you'd not want to write JASM IL by hand, and leave the generation to the frontend.

### Sections

A JASM file consists of 4 sections. Some sections are actively used in every file, some
are more useful for specific targets. But in any case, every JASM file must contain all of
them.

#### init

`init` section is the section before the tokenizer reaches `.prep`. Everything in this
section is discarded unless it is `.prep`, which marks the start of the preparation section.
This section can be used for small notes, such as the author of the file or some information
created by the frontend that uses JASM IL.

#### prep

Preparation section is the section residing between `.prep` and `.body` tokens. This section
is especially important for executable targets because the instructions `org, sts` and `sth`
are special to this section and the executable targets. The `imp` instruction that marks the
given library as a runtime library is also special to this section, but it can be present in
every file unlike `org, sts` and `sth`.

##### org

`org <symbol_name>`

Marks the given symbol as the entry point of the program. This instruction can only be used
once accross every input file. The given symbol might not be present in the same file,
but `sts` and `sth` must be. The ordering of these three instructions is important and it must
be exactly as it follows:

```
org
sts
sth
```

##### sts

`sts <size>`

Sets the stack size of the program. The reason for this is to ensure that the script will not
use more memory than the programmer indicates. This boosts the performance of the script since
it'll never have to allocate and copy memory. Also a security restriction to ensure that the
script will execute in it's own context.

Since the IL would usually be generated by the frontend, the stack size and heaps size would also
be calculated by the frontend.

##### sth

`sth <size>`

Sets the heap size of the program. Be aware that unlike the stack, heap size must be a multiple
of 8, due to the way allocation mapping works.

##### imp

`imp <path/to/shared/lib.shd`

Adds the given library file to the current context, indicating that it is a runtime assembly. So
that the linker can check it's symbols in case of an unknown symbol being left after the linking
process.

#### body
    
Body section is the section residing between `.body` and `.end` tokens. The actual code that gets
assembled is contained within this section. There isn't much to tell about this section since it
just contains code. See [syntax](#syntax).

#### end

End section is the section residing after `.end` token and EOF. Everything here is ignored.

### Registers

JASM utilizes a couple of registers, some being "system" registers and some being general purpose.

```
eax;    u32;     stores return values and returned pointers
ebx;    u32;     general purpose. main purpose is storing addresses
ecx;    u32;     general purpose. 
edx;    u32;     general purpose. 
esi;    u32;     general purpose. 
edi;    u32;     general purpose. 

al;     ubyte;      stores return values. might be used for temporal bytes and bools.
bl;     ubyte;      general purpose. might be used for temporal bytes and bools.
cl;     ubyte;      general purpose. might be used for temporal bytes and bools.
dl;     ubyte;      general purpose. might be used for temporal bytes and bools.

pc;     u32;     Program Counter. 
sp;     u32;     Stack Poiner. 

flg;    ubyte;      Program Flags
```

Technically every single register is modifiable, and the programmer potentially can use this to
their own benefits. I don't know how but I think such thing is possible.

Although the registers are mostly general purpose, I designed JASM to be a stack-based language,
so the sole purpose of registers might be just storing pointers and arbitrary values in some cases
(such as storing parameter size when creating a callstack). In the syntax section you'll learn more
about their uses.

### Syntax

JASM syntax is pretty simple, even for an IL. It can broadly be divided into three subheaders:
comments, symbols and instructions.

#### Comments

There are no single line comments in JASM. Every comment is multi line. A comment block starts with
a hashtag (#) symbol and ends with, again, a hashtag (#) symbol.

```
.prep
.body
    #A comment can be like this too#

    #
        or like 
        this
    #

    # or this
    # !!!!BUT HERE IS NOT A COMMENT!!!
.end
```

#### Symbols

Symbols are a crucial part of JASM. Through them we're able to have compile time constants, functions
and conditionals.

```
.prep
    org main
    sts 512
    sth 0
.body
    helloWorld: rom "Hello World" ; #this acts like a compile time constant#

    main:
        #here, main is the entry symbol of the program#

    someFunc:
        #
            some code here
        #
.end
```

They actually just mark a specific position in ROM (the assembled bytecode itself), so that it can then
be referenced from somewhere else.

#### Instructions

As for now, there are 123 instructions present in JASM. However many of these instructions are
actually variants of some parent instruction. So, when writing JASM IL, There are actually
40 instructions present in JASM IL, thanks to the use of `modeflags`. The assembler then turns
these instructions to their sub instructions based on their modeflags.

##### Modeflags

The modeflags are not many, but they allow us to write the generic forms of instructions instead of their
"type" dependent forms. There are 4 types of modeflags: numeric, memory, register and comparison.

Modeflags take a prefix symbol to form a token, those prefixes are `%` and `&`.

Below are all the modeflags, categorized.

```
NoMode = 0x00

NumericModeFlags:
    Int (%i) = 0x01
    Float (%f) = 0x02
    Byte (%b) = 0x03
    UInt (%ui) = 0x04
    UByte (%ub) = 0x05

MemoryModeFlags:
    Stack (%s) = 0x06
    Heap (%h) = 0x07

RegisterModeFlags:
    &eax = 0x08
    &ebx = 0x09
    &ecx = 0x0A
    &edx = 0x0B
    &esi = 0x0C
    &edi = 0x0D
    &pc = 0x0E
    &sp = 0x0F
    &bp = 0x10
    &al = 0x11
    &bl = 0x12
    &cl = 0x13
    &dl = 0x14

CompareModeFlags:
    %les = 0x15
    %gre = 0x16
    %equ = 0x17
    %leq = 0x18
    %geq = 0x19
    %neq = 0x1A
```

Some of these modeflags are just for pure visuals, such as %ui and %ub. Since JASM uses two's complement
whenever you use %ui, you can also use %i and nothing will change. But nevertheless, it makes it easier to
read the code.

##### List of Instructions

> IMPORTANT NOTE:
>   I'm writing this part from time to time since it's way too boring to do. So if you
>   can't find what you're looking for here, I apologize for the inconvinience.

###### nop

`nop` -> `0x00`

###### stc

`stc <numeric_mode> <value>`
`stc %i/%ui/%f <value>`        -> `stt <4bytes>`
`stc %b/%ub <value>`           -> `ste <byte>`

Pushes the given constant to stack. `stt` denotes `STore Thirtythree` and `ste` denotes 
`STore Eight`.

`stc <mode> <symbol>`
`stc %i/%ui/%f <symbol>`       -> `stts <4bytes> (marked for linker)`
`stc %b/%ub <symbol>`          -> `stes <4bytes> (marked for linker)`

Pushes a constant from given symbol to stack. `stts` denotes `STore Thirtythree from Symbol`
and `stes` denotes `STore Eight from Symbol`. Given 4bytes are the positions that given symbols
denote. If given symbol doesn't exist, the 4bytes are set to zero and are marked for linker.

`stc <float>`                  -> `stt <4bytes> -> 0x01 <4bytes>`
`stc <integer>`                -> `stt <4bytes> -> 0x01 <4bytes>`

Pushes given constant to stack.

```
Example:

.prep
.body

    constant: rom 21 ; #we'll see rom instruction later#

    stc %i 0x15     #store the 32-bit integer 21 on stack#
    stc 21          #same as above#
    stc %i constant #same as above at runtime, since symbol constant marks the integer 21#
.end
```

###### ldc

`ldc <numeric_mode>`
`ldc %i/%ui/%f`        -> `ldt`
`ldc %b/%ub`           -> `lde`

Loads the value at the top of the stack to memory at address stored on &ebx.
`ldt` denotes `LoaD Thirtythree` and `lde` denotes `LoaD Eight`.

```
Example:

.prep
.body
    #let &ebx contain some address#
    stc %i 5 #we stored 5 on stack#
    ldc %i   #now we'll store 5, which is on top of the stack at the address on &ebx#
.end
```

###### rda

`rda <register_mode>`  -> `rdr <register_mode>`

Reads the given register, and stores its value on stack. `rdr` stands for `ReaD Register`

`rda %i/%ui/%f`        -> `rdt`
`rda %b/%ub`           -> `rde`

Reads the value from memory at address stored on &ebx, and stores it on stack.
`rdt` denotes `ReaD Thritythree` and `rde` denotes `ReaD Eight`.

```
Example:

.prep
.body
    #let &ecx contain the value 15, in decimal#
    #and &ebx contain an address, in which the value 21 is stored#
    rda &ecx #now 15 is on top of the stack#
    rda %i   #now 21 is on opt of the stack#
    #so stack is now 15-21#
```

###### mov

`mov <register_mode>`   -> `movs <byte>`

MOVes the top n bytes depending on the given register size from the top of the stack to given register.

`mov <register_mode> <register_mode>`   -> `movr <byte> <byte>`

MOVes the value of the first register to second one.

`mov <value> <register_mode>`
`mov %b/%ub <register_mode>`          -> `movc <byte> <byte>`
`mov %i/%ui/%f <register_mode>`       -> `movc <byte> <4bytes>`

MOVes the given constant value to given register.

###### add, sub, mul, div

These arithmetic operations differ just in name, so I put them under the same title.
Where [op] <- {add, sub, mul, div}

`[op] <numeric_mode>`
`[op] %i/%ui`           -> `[op]i`
`[op] %b/%ub`           -> `[op]b`
`[op] %f`               -> `[op]f`

Pops and applies [op] to values from the top of the stack, then pushes the result.

`[op] <numeric_mode> <register_mode> <register_mode>`
`[op] %i/%ui`           -> `[op]ri <byte> <byte>`
`[op] %b/%ub`           -> `[op]rb <byte> <byte>`
`[op] %f`               -> `[op]rf <byte> <byte>`

Adds the first register to second.

###### adds, subs, muls, divs

These arithmetic operations differ just in name, so I put them under the same title too.
Where [op] <- {adds, subs, muls, divs}

`[op] <numeric_mode>`
`[op] %i/%ui`           -> `[op]i`
`[op] %b/%ub`           -> `[op]b`
`[op] %f`               -> `[op]f`

Applies [op] to values from the top of the stack without popping, then pushes the result.

###### inc, dcr

These too differ just in name.
Where [op] <- {inc, dcr}

`[op] <numeric_mode> <value>`
`[op] %i/%ui <value>`           -> `[op]i <4bytes>`
`[op] %b/%ub <value>`           -> `[op]b <byte>`
`[op] %f <value>`               -> `[op]f <4bytes>`

Increments the value at the top of the stack <value> times.

`[op] <numeric_mode> <register_mode> <value>`
`[op] %i/%ui <register_mode> <value>`           -> `[op]ri <byte> <4bytes>`
`[op] %b/%ub <register_mode> <value>`           -> `[op]rb <byte> <byte>`
`[op] %f <register_mode> <value>`               -> `[op]rf <byte> <4bytes>`

Increments the value in given register <value> times. Note that if you specify a sizewise missmatching
mode between <numeric_mode> and <register_mode>, assembler will throw an error. You can use
%b/%ub with 32-bit registers but you can't use %i/%ui/%f with 8-bit registers.

###### incs, dcrs

There too differ just in name.
Where [op] <- {incs, dcrs}

`[op] <numeric_mode> <value>`
`[op] %i/%ui <value>`           -> `[op]i <4bytes>`
`[op] %b/%ub <value>`           -> `[op]b <byte>`
`[op] %f <value>`               -> `[op]f <4bytes>`

Increments the value at the top of the stack <value> times and pushes the result, without modifying
the value on top of the stack.

mcp
and
or
nor
swp
dup
raw
rom
inv
invs
cmp
pop
jmp
swr
dur
rep
alc
pow
sqr
cnd
cal
ret
del

### Atomic Types

Atomic types are the types that are natively supported by JASM which can be manipulated using
instructions. Arrays, lists, maps, strings, layouts are not atomic since JASM can't directly
manipulate them. Below are the atomic types of JASM.

```
Int32
UInt32
Byte
UByte
Float (Single Precision)
```

And that's all. All JASM instructions are built around these types. Complex types besides these
are compiler illusions. Which are up to you to create since CSLB is only a backend.

### Splitting Code Accross Files

Each JASM file is independent of each other. If you want to split code accross files,
you can directly use the symbols you wish to use since as long as the file that contains the symbol
passes from the linker, linker will handle the rest. So no need to indicate the symbol as
extenral, include headers or anything.

Although it looks like it can lead to terrible debugging sessions and confusions when reading
the code, remember that JASM is not designed to be used directly. The compilers will do the checks
anyway loong before sending the generated JASM code to the assembler so why check it again?

#### Referencing Unknown Symbols
```
main.jasm

.prep
    org main
    sts 0
    sth 0
.body
    main:
        mov 0 &bl #bl is guaranteed to be initially zero but whatever#
        cal test
.end
```

```
test.jasm

.prep
.body
    test:
        mov 5 &eax
        ret
.end
```

This is pefreclty fine as long as you include both files when assembling.

It is also possible to use unknown symbols which are present in the libraries you'll link against. 
Linker will check the symbols and resolve the issues.

## Why This Documentation Does Not Explain How The IL Works?

JASM (this repo) is the assembler and linker part of the CSLB. So IL is nothing but a bunch of symbols
to be converted to bytecode format at this point. The execution and how it works part is handled
by (CSR)[https://github.com/ysufender/CSR] (link can be down due to me not making the repo public) so
the execution part will be explained in CSR's documentation.

But if your question is "why" rather than "how", the answer is "I too don't know" 95% of the time. For
the 5%, you can contact me and ask anything. Probably I'll learn more from you than you from me so a win/win
situation.
