
file_name = input("filename: ")
address = 512
label_table = {}
generated_bytes = []

#pass 1, figure out labels and macros

with open(file_name) as file:
    lines = file.readlines()
    
if not any("main:" in line.lower() for line in lines):
    print("No entry point found in program!")
    exit()

for line in lines:

    line = line.split(';')[0]
    line = line.strip().lower()

    tokens = line.replace(',', ' ').split()

    if not tokens:
        continue

    if ":" in tokens[0]:
        label_name = tokens[0].rstrip(":")
        label_table[label_name] = address
        if tokens[-1].endswith(':'): 
            continue

    elif tokens[0] == ".byte":
        num_bytes = len(tokens) - 1 
        address += num_bytes
        continue 

    if tokens[0] == "long":
        address += 4
    else:
        address += 2
    


# pass 2, figure out opcodes

for line in lines:

    line = line.split(';')[0]
    line = line.strip().lower()

    tokens = line.replace(',', ' ').split()

    if not tokens:
        continue

    if tokens[0] == ".byte":
        data_values = tokens[1:] 

        for byte_str in data_values:
            byte_value = int(byte_str, 0) 
            generated_bytes.append(byte_value)

        continue

    elif tokens[0] == "scd":
        n = int(tokens[1], 0)
        b = [0x00, 0xC0 | n]
        generated_bytes.extend(b)

    elif tokens[0] == "scu":
        n = int(tokens[1], 0)
        b = [0x00, 0xD0 | n]
        generated_bytes.extend(b)

    elif tokens[0] == "cls":
        b = [0x00, 0xE0]
        generated_bytes.extend(b)

    elif tokens[0] == "ret":
        b = [0x00, 0xEE]
        generated_bytes.extend(b)

    elif tokens[0] == "scr":
        b = [0x00, 0xFB]
        generated_bytes.extend(b)

    elif tokens[0] == "scl":
        b = [0x00, 0xFC]
        generated_bytes.extend(b)

    elif tokens[0] == "lores":
        b = [0x00, 0xFE]
        generated_bytes.extend(b)

    elif tokens[0] == "hires":
        b = [0x00, 0xFF]
        generated_bytes.extend(b)

    elif tokens[0] == "jmp":
        target = tokens[1]

        if target in label_table:
            nnn = label_table[target]
        else:
            print(f"Target label: {target} not found!")
            continue

        b = [0x10 | (nnn >> 8) & 0x0f, nnn & 0xFF]
        generated_bytes.extend(b)

    elif tokens[0] == "call":
        target = tokens[1]

        if target in label_table:
            nnn = label_table[target]
        else:
            print(f"Target label: {target} not found!")
            continue
        
        b = [0x20 | (nnn >> 8) & 0x0F, nnn & 0xFF]
        generated_bytes.extend(b)

    elif tokens[0] == "se":
        x = int(tokens[1], 0)
        nn = int(tokens[2], 0)
        b = [0x30 | (x & 0x0F), nn]
        generated_bytes.extend(b)

    elif tokens[0] == "sne":
        x = int(tokens[1], 0)
        nn = int(tokens[2], 0)
        b = [0x40 | (x & 0x0F), nn]
        generated_bytes.extend(b)

    elif tokens[0] == "ser":
        x = int(tokens[1], 0)
        nn = int(tokens[2], 0)
        b = [0x50 | (x & 0x0F), (y << 4) | 0x00]
        generated_bytes.extend(b)

    elif tokens[0] == "ld":
        x = int(tokens[1], 0)
        nn = int(tokens[2], 0)
        b = [0x60 | (x & 0x0F), nn]
        generated_bytes.extend(b)

    elif tokens[0] == "addi":
        x = int(tokens[1], 0)
        nn = int(tokens[2], 0)
        b = [0x70 | (x & 0x0F), nn]
        generated_bytes.extend(b)

    elif tokens[0] == "ldr":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x00]
        generated_bytes.extend(b)

    elif tokens[0] == "or":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x01]
        generated_bytes.extend(b)

    elif tokens[0] == "and":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x02]
        generated_bytes.extend(b)        

    elif tokens[0] == "xor":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x03]
        generated_bytes.extend(b)        

    elif tokens[0] == "add":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x04]
        generated_bytes.extend(b)        

    elif tokens[0] == "sub":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x05]
        generated_bytes.extend(b)        

    elif tokens[0] == "shr":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x06]
        generated_bytes.extend(b)        

    elif tokens[0] == "subn":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x07]
        generated_bytes.extend(b)        

    elif tokens[0] == "shl":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x80 | (x & 0x0F), (y << 4) | 0x0E]
        generated_bytes.extend(b)        

    elif tokens[0] == "sner":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0x90 | (x & 0x0F), (y << 4) | 0x00]
        generated_bytes.extend(b)  

    elif tokens[0] == "ldi":

        target = tokens[1]

        if target in label_table:
            nnn = label_table[target]
        else:
            nnn = int(tokens[1], 0)

        b = [0xA0 | (nnn >> 8), (nnn & 0xFF)]
        generated_bytes.extend(b) 

    elif tokens[0] == "jmpv":
        nnn = int(tokens[1], 0)
        b = [0xB0 | (nnn >> 8), (nnn & 0xFF)]
        generated_bytes.extend(b) 

    elif tokens[0] == "random":
        x = int(tokens[1], 0)
        nn = int(tokens[2], 0)
        b = [0xC0 | (x & 0xF) | nn]
        generated_bytes.extend(b) 

    elif tokens[0] == "draw":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        n = int(tokens[3], 0)
        b = [0xD0 | x, (y << 4) | n]
        generated_bytes.extend(b) 

    elif tokens[0] == "bigdraw":
        x = int(tokens[1], 0)
        y = int(tokens[2], 0)
        b = [0xD0 | x, (y << 4) | 0x00]
        generated_bytes.extend(b) 

    elif tokens[0] == "skp":
        x = int(tokens[1], 0)
        b = [0xE0 | x, 0x9E]
        generated_bytes.extend(b) 

    elif tokens[0] == "sknp":
        x = int(tokens[1], 0)
        b = [0xE0 | x, 0xA1]
        generated_bytes.extend(b) 

    elif tokens[0] == "long":
        nnnn = tokens[1]
        b = [0xF0, 0x00, (nnnn >> 8) & 0xFF, nnnn & 0xFF]
        generated_bytes.extend(b) 

    elif tokens[0] == "plane":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x01]
        generated_bytes.extend(b)

    elif tokens[0] == "getdelay":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x07]
        generated_bytes.extend(b) 

    elif tokens[0] == "wait":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x0A]
        generated_bytes.extend(b) 

    elif tokens[0] == "delay":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x15]
        generated_bytes.extend(b) 

    elif tokens[0] == "sound":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x18]
        generated_bytes.extend(b) 

    elif tokens[0] == "vxi":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x1E]
        generated_bytes.extend(b) 

    elif tokens[0] == "smallhex":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x29]
        generated_bytes.extend(b) 

    elif tokens[0] == "bighex":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x30]
        generated_bytes.extend(b) 

    elif tokens[0] == "bcd":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x33]
        generated_bytes.extend(b) 

    elif tokens[0] == "store":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x55]
        generated_bytes.extend(b) 

    elif tokens[0] == "read":
        x = int(tokens[1], 0)
        b = [0xF0 | x, 0x65]
        generated_bytes.extend(b) 


print(label_table)

for byte_value in generated_bytes:
    print(f"{byte_value:02X}", end=" ")
print()

bytes_to_write = bytes(generated_bytes)

output_name_temp = file_name.split("/")
output_name = "output/" + output_name_temp[1].split(".")[0] + ".ch8"

with open(output_name, "wb") as f:
    f.write(bytes_to_write)


