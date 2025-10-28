to compile on linux you must have raylib and make/g++ installed

run `make`
and to use the emulator just run 
`./chip8 pathtorom.ch8`

---------

included is a small chip8 assembler, please don't use it you won't like it 

but if you do want to use it, just write your assembly program in the deep dark `dev/chasm` as a .chasm file, and in `dev`
run the command `python assembler.py`.
type in the path to your source code, and it should assemble into a .ch8 file in `./output` (the rom i made while testing is included lol)
