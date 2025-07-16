# JASM - Just an Assembly 

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

## Quickstart

### Installation

You can either grab the compiled binaries from the release section (if there is any), or build JASM from source. I recommend
building from source since it's pretty easy.

#### Building From The Source

See [BUILD.md](docs/BUILD.md)

### Basic CLI Usage

Here is the helper text from the current version of JASM:

```

Just an Assembler (JASM)
        Description: Assembler and linker for JASM
        Version: 1.0.0
        Use Pipelines: Unavailable
        Build Mode: CLI

Available Flags:
        --help , -h : Print this help text.
        --version , -v : Print version.

        --silent , -s : Disables SOME outputs. So it's less painful to look at the screen.
        --single , -S : Assemble each file provided by '--in' flag but do NOT link.
        --out <value>, -o : Place the output file to specified path if flag '--single' is not set.
        --lib-type <value> : If desired output is a library, specify the type. (either shared 'shd' or static 'stc')

        --in <..params..>, -I : Files to assemble and (optionally) link. The first entry must contain the entry point, if output is executable.
        --libs <..params..>, -L : Libraries used and to be linked.

        --pipelines , -p : Use pipeline-streams instead of intermediate files.
        --working-dir <value>, -w : Set the working directory of the assembler.
        --redirect-stdout <..params..>, -r : Redirect stdout and stderr to given files. If only one is provided, both get redirected to it.


        WARNING: In single mode, each file will be assembled as a static library by default. You can specify it with `--libType` flag.

```

Assuming you have a project structure that looks like this:

```
./
|_ src/
| |_ main.jasm
| |_ second.jasm
|_ lib/
| |_ randomlib.jasm
|_ out/
```

You can generate an `out.jef` file in the `out` directory using: 

`jasm -o "out/out.jef" -I "src/main.jasm" "src/second.jasm" "lib/randomlib.jasm"`

Be aware that if the extension you provided for the output does not match the build type, JASM will append the 
correct extension at the end of the name you've provided.

Also note that the quotes are optional as long as the string doesn't contain any spaces.

### JASM Documentation 

If you want to know more about how JASM works you can check the `.txt` files under the `docs` directory, or 
you can start reading the [docs](docs/DOCUMENTATION.md)

## Footnotes

The licenses, readmes and citations for every library used in this project, lies within its own directory
under `lib`.
