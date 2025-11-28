## TODO

## Assembler

Create a prep section instruction with form `ffi "return_t fnname(type params)"` and then
add those C function signature list to AssemblyInfo in the form of (name, signature) pairs.

### Instructions

- [ ] Complex Instructions
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

## Linker

- [ ] Shared libraries
