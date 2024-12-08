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
the name of the IL, bytecode, assembler and linker altogether. The runtime is CSR (Common Script Runtime, pronunced as Caesar),
it has its own repo.

## Installing JASM 

You can either grab the compiled binaries from the release section or build JASM from source.
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
    - [The JASM IL](#the-jasm-il)
        - [Sections](#sections)
            - [init](#init)
            - [prep](#prep)
            - [body](#body)
            - [end](#end)
        - [Registers](#registers)
        - [Syntax](#syntax)
	        - [Comments](#comments)
            - [Symbols](#symbols)
            - [Instructions](#instructions)
        - [Atomic Types](#atomic-types)
        - [Splitting Code Accross Files](#splitting-code-accross-files)
            - [Referencing Unknown Symbols](#referencing-unknown-symbols)
	- [Why This Documentation Does Not Explain How The IL Works?](#why-this-documentation-does-not-explain-how-the-il-work)
	
## The Project Structure

The JASM project includes the assembler(s) and the linker(s) for the JASM IL.

./
|_ docs/ -----------------------> contains documentation `.md`s, concept `.jasm`s and various descriptive `.txt` files.
|_ include/
| |_ bytemode/
| | |_ assembler/
| | |_ linker/
| |_ extensions/
|_ lib/ ------------------------> contains third-party libraries
|_ src/
  |_ bytemode/ -----------------> the project is designed to allow future improvements and additions like assembler for assembling 
  | |_ assembler/ --------------> to machine code instead of byte that's why the `assembler` and `linker` reside under `bytemode`
  | |_ linker/  
  |_ core/ ---------------------> the core functions that doesn't change depending on the target mode
  |_ extensions/

### The CLI

The CLI is mostly handler by the [CLI Parser](https://github.com/The2ndSlimShady/CLIParser), so if you want to know how it works you have to check its source.
As of the CLI usage, basic CLI usage is covered in [README.md](../README.md#basic-cli-usage). If you wan't to know more about
the CLI parameters and flags, this section covers that.

#### silent

`jasm --silent` or `jasm -s` (with a lowercase `s`)

The logging system in the JASM project has four modes: `LOG`, `LOGD`, `LOGW` and `LOGE`.

The modes `LOG`, `LOGD` and `LOGW` are disabled when the `silent` flag is set. The `LOGE` is only active
when its in `Medium` or `High` mode. Any unhandled exception is non-silent no matter the `silent` flag.

#### single

`jasm --single` or `jasm -S` (with an uppercase `S`)

By default, JASM first assembles all the input files and then links them together and creates a final file depending on the 
`--lib-type` flag. However when `single` flag is set, JASM stops after the assembling process and only creates "`jo (jasm object)`" 
files. Each of these object files follow the `static library` structure and can then later be used as static libraries.

#### out

`jasm --out <value>` or `jasm -o <value>` (with a lowercase `o`)

By default, JASM will use the first entry from the input files to generate an output name based on it and target output type.
The ouput will be placed in the same directory as the first input file. Optionally you can provide a name along with a path for
the output file. Be aware that if the name you've provided contains an extension that doesn't match with the target output type,
JASM will simply append the correct extension at the end of the name you've provided.

For example if you provide `out.jef` for a target static library, the output will be `out.jef.stc`.

#### lib-type

`jasm --lib-type <value>` (where `value` can be either `stc` or `shd`)

By default, JASM will assume that the target output format is executable unless you specify otherwise via `lib-type` flag.
The flag accepts two possible values, one being `stc` for `STatiC` and the other `shd` for `SHareD`. If a `lib-type` is provided
but is not `stc` or `shd`, JASM will proceed by assuming it as an executable.
