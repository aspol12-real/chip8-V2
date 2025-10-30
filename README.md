to compile on linux you must have raylib and make/g++ installed

run `make` to build,
and to use the emulator just run 
`./chip8 pathtorom.ch8`

---------

included is a small chip8 assembler, please don't use it you won't like it 

but if you do want to use it, just write your assembly program in the deep dark `dev/chasm` as a .chasm file, and in `dev`
run the command `python assembler.py`.
type in the path to your source code, and it should assemble into a .ch8 file in `./output` (the rom i made while testing is included lol)


(mnemonics)


``` opcodes
cls = 00E0  
ret = 00EE  
jmp nnn = 1nnn  
call nnn = 2nnn  
se VX, NN = 3xnn  
sne VX, NN = 4xnn  
se VX, VY = 5xy0  
ld VX, NN = 6xnn  
add VX, NN = 7xnn  
ld VX, VY = 8xy0  
or VX, VY = 8xy1  
and VX, VY = 8xy2  
xor VX, VY = 8xy3  
add VX, VY = 8xy4  
sub VX, VY = 8xy5  
shr VX = 8xy6  
subn VX, VY = 8xy7  
shl VX = 8xyE  
sne VX, VY = 9xy0  
ldi, NNN = Annn  
jmp v0, NNN = Bnnn  
random VX, NN = Cxnn  
draw VX, VY, N = Dxyn  
skp VX = Ex9E  
sknp VX = ExA1  
gdelay VX = Fx07  
wait VX = Fx0A  
delay VX = Fx15  
sound VX = Fx18  
vxi VX = Fx1E  
smallhex VX = Fx29  
bcd VX = Fx33  
str VX = Fx55  
read VX = Fx55  
```
