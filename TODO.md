## TODO

### Redesigns

- [x] I made a bunch of instructions available to user. Might just reduce it and use modeflags
instead. Also might just reduce function count (the implementations in instructions.cpp)
But for now it's also good for unit tests.
- [x] stc/ldc et cetera... doesn't need the type specifiers. They just need to know if type
is 32 or 8 bits

### Assembler 

- [x] ROM manipulation
    - [x] Store raw sequence of data in ROM

- [x] Stack Manipulation
    - [x] Constant to stack
    - [x] Register to stack
    - [x] Heap to stack
    - [x] Stack to stack (copy)
    - [x] ROM to stack
    - [x] Duplicate the top 4-bytes 
    - [x] Duplicate the top byte 
    - [x] Duplicate the top range of bytes 
    - [x] Swap the top 2 4-bytes
    - [x] Swap the top 2 byte
    - [x] Swap the top range of bytes
    - [x] Push a range of raw data
    - [x] Read a range of raw data from symbol
    - [x] Read a range from heap
    - [x] Pop 4-bytes from stack
    - [x] Pop 2-bytes from stack
    - [x] Set a range of stack to given value

- [x] Heap Manipulation
    - [x] Constant to heap (workaround by stc)
    - [x] Register to heap (workaround by rda)
    - [x] Stack to heap
    - [x] Heap to heap (copy)
    - [x] ROM to heap (workaround by stc)
    - [x] Read a range from stack
    - [x] Set a range of heap to given value
    - [x] Allocate memory on heap

- [x] Register Manipulation
    - [x] Constant to register
    - [x] Register to register
    - [x] Stack to register
    - [x] Heap to register (workaround by rda)
    - [x] ROM to register (workaround by stc)
    - [x] Swap two registers
    - [x] Invert a register

- [x] Arithmetic and Logic Operations 
    - [x] Add
        - [x] Add two values from stack
        - [x] Add two values from stack, but don't pop
        - [x] Add two registers, set the second
    - [x] Multiply
        - [x] Multiply two values from stack
        - [x] Multiply two values from stack, but don't pop
        - [x] Multiply two registers, set the second
    - [x] Divide
        - [x] Divide two values from stack
        - [x] Divide two values from stack, but don't pop
        - [x] Divide two registers, set the second
    - [x] Increment
        - [x] Increment the value on stack
        - [x] Increment the value on stack, and push it
        - [x] Increment a register
    - [x] Decrement
        - [x] Decrement the value on stack
        - [x] Decrement the value on stack, and push it
        - [x] Decrement a register
    - [x] And
        - [x] On stack, push the result
        - [x] On registers, set the second
    - [x] Or 
        - [x] On stack, push the result
        - [x] On registers, set the second
    - [x] Nor 
        - [x] On stack, push the result
        - [x] On registers, set the second
    - [x] Invert 
        - [x] On stack
        - [x] On stack, push the result
        - [x] On register
    - [x] Compare (lesser, greater, equal, lesser-equal, greater-equal, not-equal)
        - [x] On stack, set register
        - [x] On register, set second
        
- [ ] Complex Instructions
    - [ ] Branching
        - [x] Branch
            - [x] Indirect branch
        - [x] Conditional branch
            - [x] Conditional indirect branch
        - [x] Call
            - [x] Change the way cal works. Instead of passing size, store it on an 8-bit register
            - [x] Return
        - [ ] System Calls (calling to c/c++ in our case)
    - [ ] Multithreading
        - [ ] Create and execute independent threads
        - [ ] Create and execute shared threads (registers are shared. and maybe stack. I'm not sure)
    - [ ] Dumping CPU state (dumping current execution context in our case)
    - [ ] Loading CPU state (loading current execution context in our case)
    - [ ] Complex integer/float arithmetic
        - [x] Exponents
        - [x] Taking roots
        - [x] Square root
        - [ ] Inverse square root
        - [ ] Transcendental functions

### Linker

- [x] Basic backtracking
- [ ] Shared libraries

### Bugs

- [ ] For some reason, for lib1 that calls functions from lib2 (so is linked with lib2), when the exe calls that
        function from lib1 that calls from lib2, it results in an infinite loop off StackOverflow. This mmight be
        a CSR problem or a JASM problem, I don't know.
