## TODO

### Redesigns

- [x] I made a bunch of instructions available to user. Might just reduce it and use modeflags
instead. Also might just reduce function count (the implementations in instructions.cpp)
But for now it's also good for unit tests.
- [x] stc/ldc et cetera... doesn't need the type specifiers. They just need to know if type
is 32 or 8 bits

### The Instruction Set Standards 

- [x] Stack Manipulation
    - [x] Constant to stack
    - [x] Register to stack
    - [x] Heap to stack
    - [x] Stack to stack (copy)
    - [x] ROM to stack
    - [x] Duplicate the top 4-bytes 
    - [x] Duplicate the top byte 
    - [x] Swap the top 2 4-bytes
    - [x] Swap the top 2 byte
    - [x] Push a range of raw data

- [x] Heap Manipulation
    - [x] Constant to heap (workaround by stc)
    - [x] Register to heap (workaround by rda)
    - [x] Stack to heap
    - [x] Heap to heap (copy)
    - [x] ROM to heap (workaround by stc)

- [x] Register Manipulation
    - [x] Constant to register
    - [x] Register to register
    - [x] Stack to register
    - [x] Heap to register (workaround by rda)
    - [x] ROM to register (workaround by stc)
    - [x] Swap two registers

- [ ] Arithmetic and Logic Operations 
    - [x] Add
        - [x] Add two values from stack
        - [x] Add two values from stack, but don't pop
        - [x] Add two registers, set the second
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
    - [ ] Not 
        - [ ] On stack
        - [ ] On stack, push the result
        - [ ] On register
    - [ ] Compare (lesser, greater, equal, lesser-equal, greater-equal, not-equal)
        - [ ] On stack, set register
        - [ ] On stack, push result
        - [ ] On register, set second
    - [ ] Branching
        - [ ] Branch
        - [ ] Conditional branch
        - [ ] Indirect branch
        - [ ] Call
        - [ ] System Calls (calling to c/c++ in our case)
    - [ ] Multithreading
        - [ ] Create and execute independent threads
        - [ ] Create and execute shared threads (registers are shared. and maybe stack. I'm not sure)
    - [ ] Complex Instructions
        - [ ] Dumping CPU state (dumping current execution context in our case)
        - [ ] Loading CPU state (loading current execution context in our case)
        - [ ] Complex integer/float arithmetic
            - [ ] Exponents
            - [ ] Taking roots
            - [ ] Square root
            - [ ] Inverse square root
            - [ ] Transcendental functions
