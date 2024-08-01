# First Steps

Create the base CLI tool: 
- [x] Print Helper 
- [x] Assembler Options Context 
- [x] Proper Base Structure 
- [x] Some Utility Functions That Are Probably Will Be Needed 

Create the Assembler: 
- [x] Set up the AssemblyInfo class.
- [x] Serialization

CREATE THE FUCKING ASSEMBLY LANGUAGE FIRST IDIOT

## On Assembly

- A symbol name can be maximum uint16_t in size.
- Assembly is designed to be 32-bit so the max integer and the max memory is calculated by uint32_t

> NOTE: 
>   The reason I chose uint16_t and uint32_t is that because the files are getting too big.
>   A library has to contain the assembly info containing its symbols' names and addresses.
>   If I use size_t while writing name sizes, well, that's a bit too much. So 16 bits. Yeah.

> !!! TODO !!!
> (Plans on symbols)
>   I'm planning to not use raw string in AssemblyInfo. I'll create a hash function and hash the
>   symbol names with it, then use their integer hashes as keys in places. This way both the memory
>   fottprint and the file size problem will be solved.

### Registers

| Name | Size   | Accesibility | Description               |
|------|--------|--------------|---------------------------|
| PS   | 8-bit  | Pub-RW       | Program Status Code       |
| SF   | 8-bit  | Res-W, Pub-R | Operation Status Flags    |
| PC   | 32-bit | Res-RW       | Current Point in Bytecode |
| SP   | 32-bit | Pub-R, Res-W | Current Point in Stack    |

### Supported Data Types

```
array   ([u]byte, [u]int, float) #Basically raw data#
int     (32-bit, signed)
uint    (32-bit, unsigned)
float   (32-bit, signed)
byte    (8-bit, signed)
ubyte   (8-bit, unsigned)
```

### Instructions

Refer to (instructions.txt)[docs/instructions.txt]
