## Developing environment
Ubuntu 22.04, clang 14, cmake 3.22, C++ 17

## How to build the simulator
- easily, just `make` on the directory **/Simulator**
- advancedly, `mkdir build`, then `cd build`, `cmake ..`, `make`

# How to run the simulator
`./build/Simulator [your MIPS assembly code file]` , e.g., `./build/Simulator ./tests/CODE1.S`
# Usage:
- v [i | p | r | b | m | s]: display instructions | pipelines | registers | breakpoints | memory | statistics  
- b [instruction index] : set breakpoint at instruction of the index  
- s : single cycles  
- r : run to the end or breakpoint  
- q : quit the simulator  