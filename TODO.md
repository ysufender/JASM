# First Steps

~~Create the base CLI tool:~~ 
~~- [x] Print Helper~~ 
~~- [x] Assembler Options Context~~ 
~~- [x] Proper Base Structure~~ 
~~- [ ] Some Utility Functions That Are Probably Will Be Needed~~ 

~~Create the Assembler:~~ 
~~- [ ] Set up the AssemblerOutput class~~.

CREATE THE FUCKING ASSEMBLY LANGUAGE FIRST IDIOT

## On Assembly

- A symbol name can be maximum uint16_t in size.
- Assembly is designed to be 32-bit so the max integer and the max memory is calculated by uint32_t

> NOTE: 
>   The reason I chose uint16_t and uint32_t is that because the files are getting too big.
>   A library has to contain the assembly info containing its symbols' names and addresses.
>   If I use size_t while writing name sizes, well, that's a bit too much. So 16 bits. Yeah.

### Registers

| Name | Size   | Accesibility | Description               |
|------|--------|--------------|---------------------------|
| PS   | 8-bit  | Pub-RW       | Program Status Code       |
| SF   | 8-bit  | Res-W, Pub-R | Operation Status Flags    |
| PC   | 32-bit | Res-RW       | Current Point in Bytecode |
| SP   | 32-bit | Pub-R, Res-W | Current Point in Stack    |

### Supported Data Types

```
array   ([u]byte, [u]int, float)      ;; Always stored in heap |type_flag (char)|size (uint)|<values>|
int     (32-bit, signed)
uint    (32-bit, unsigned)
float   (32-bit, signed)
byte    (8-bit, signed)
ubyte   (8-bit, unsigned)
```

### Instructions

```
sti <constant>      ;; push int constant to stack
stf <constant>      ;; push float constant to stack
stb <constant>      ;; push byte constant to stack
stui <constant>     ;; push uint constant to stack
stub <constant>     ;; push ubyte constant to stack

ldi <constant>      ;; load constant int to heap and push address to stack 
ldf <constant>      ;; load constant float to heap and push address to stack 
ldb <constant>      ;; load constant byte to heap and push address to stack 
ldui <constant>     ;; load constant uint to heap and push address to stack 
ldui <constant>     ;; load constant ubyte to heap and push address to stack 

mov <from_addr> <to_addr>   ;; copy the value of from_addr to to_addr. from_addr remains as is.
```
