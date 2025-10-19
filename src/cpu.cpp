#include "cpu.hpp"

void cpu::init() {

    //init registers

    pc = 512;
    sp = 0;
    I = 0;

    delay = 0;
    sound = 0;

    scr.selected_plane = 3;
    scr.hires = false;

    for (int i = 0; i < 16; i++) {
        v[i] = 0;
    }

    //copy font data into memory
    for (int i = 0; i < 240; i++) {
        mem[i] = font_data[i];
    }

    scr.clear_all();

}

void cpu::load_rom(std::string rom) {

    std::ifstream file;
    file.open(rom, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Could not locate source file: " << rom <<  "\n";
        exit( 1 );
    } else {
        std::cout << "\n\nLoading rom file: " << rom << "\n";
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0);

    if (fileSize > MEM_SIZE) {
        std::cout << "Invalid ROM size: " << fileSize << " bytes\n";
        exit( 1 );
    }  

    char byte;
    int address = 0;

    while (file.get(byte)) {
        mem[address + 512] = static_cast<uint8_t>(byte);
        address++;
    }

    file.close();
}

void cpu::tick_timers() {

    if (delay > 0) {
            delay--;
        }
    if (sound > 0) {
        sound--;
    }


}





void cpu::execute() {

    uint16_t opcode = (mem[pc] << 8) | mem[pc + 1];
    uint8_t inst    = (opcode >> 12);
    uint8_t x       = (opcode & 0x0F00) >> 8;
    uint8_t y       = (opcode & 0x00F0) >> 4;
    uint8_t n       = (opcode & 0x000F);
    uint8_t nn      = (opcode & 0x00FF);
    uint16_t nnn    = (opcode & 0x0FFF);

    uint8_t keyindex = v[x];

    uint8_t random = rand() % 0xFF;

    pc += 2; //post fetch increment!

    //decode / execute

    switch (inst) {

       
        case 0x0: 

            if ((nn >> 4) == 0xC) {}
            else if ((nn >> 4) == 0xD) {}

            switch (nn) {

                case 0xE0: scr.clear_plane(); break;
                case 0xEE: ret(); break;
                case 0xFB: break; //scroll right
                case 0xFC: break; //scroll left
                case 0xFD: std::cout << "EXITING... (0x00FD)\n"; exit( 1 );
                case 0xFE: scr.hires = false; break;
                case 0xFF: scr.hires = true; break;

            }
            break;

        case 0x1: pc = nnn; break;
        case 0x2: call(nnn); break;
        case 0x3: jeq(v[x], nn); break;
        case 0x4: jneq(v[x], nn); break;
        
        case 0x5: 
            switch(n) {
                case 0x0: jeq(v[x], v[y]); break;
                case 0x2: reg_to_mem(x, y); break;
                case 0x3: reg_from_mem(x, y); break;
            }
            break;
        
        case 0x6: v[x] = nn; break;
        case 0x7: v[x] += nn; break;

        case 0x8:
            switch(n) {

                case 0x0: v[x] = v[y]; break;
                case 0x1: v[x] = (v[x] | v[y]); break;
                case 0x2: v[x] = (v[x] & v[y]); break;
                case 0x3: v[x] = (v[x] ^ v[y]); break;
                case 0x4: add(x, y); break;
                case 0x5: sub_xy(x, y); break;
                case 0x6: shift_right(x, y); break;
                case 0x7: sub_yx(x, y); break;
                case 0xE: shift_left(x, y); break;

            }
            break;

        case 0x9: jneq(v[x], v[y]); break;
        case 0xA: I = nnn; break;
        case 0xB: pc = (nnn + v[0]); break;
        case 0xC: gen_rand(nn, v[x], random); break;

        case 0xD:
            if (n == 0) {
                v[0xF] = scr.draw_sprite16(v[x], v[y], I);
            }
            else {
                v[0xF] = scr.draw_sprite8(v[x], v[y], n, I);
            }
            break;

        case 0xE:
            if (nn == 0x9E) {
                if (keyindex < 16 && keys[keyindex]) {
                    big_skip_check();
                    pc += 2;
                }
            } else if (nn == 0xA1) {
                if (keyindex < 16 && !keys[keyindex]) {
                    big_skip_check();
                    pc += 2;
                }
            }
            break;

            
        case 0xF:
            switch (nn) {

                case 0x00: word_index(); break;
                case 0x01: scr.selected_plane = x; break;
                //case 0x02: audio pattern into buffer
                case 0x07: v[x] = delay; break;
                case 0x0A: wait(x); break;
                case 0x15: delay = v[x]; break;
                case 0x18: sound = v[x]; break;
                case 0x1E: I += v[x]; break;
                case 0x29: small_hex(x); break;
                case 0x30: big_hex(x); break;
                case 0x33: BCD(x); break;
                //case 3A audio pitch 
                case 0x55: reg_to_mem(0, x); I += (x + 1); break;
                case 0x65: reg_from_mem(0, x); I += (x + 1); break;
                case 0x75: reg_to_flags(0, x); break;
                case 0x85: reg_from_flags(0, x); break;
            }
            break;
        default:
            std::cout << "unknown opcode: " << std::hex << +opcode << "\n";
    }

}






void cpu::call(uint16_t nnn) {

    if (sp >= 15) {
        std::cout << "STACK OVERFLOW";
        return;
    }
    sp++;
    stack[sp] = pc;

    pc = nnn;

}

void cpu::ret() {

    pc = stack[sp];
    sp--;

}

void cpu::jeq(uint8_t a, uint8_t b) {


    if (a == b) {
        big_skip_check(); 
        pc += 2;
    } else {
        return;
    }

}

void cpu::jneq(uint8_t a, uint8_t b) {

    if (a != b) {
        big_skip_check();
        pc += 2;
    } else {
        return;
    }

}

void cpu::reg_to_mem(uint8_t x, uint8_t y) {

    for (int i = x; i <= y; i++) {
        mem[I + i] = v[i];
    }

}

void cpu::reg_from_mem(uint8_t x, uint8_t y) {

    for (int i = x; i <= y; i++) {
        v[i] = mem[I + i];
    }
}


//arithmetic

void cpu::add(uint8_t x, uint8_t y) {

    uint16_t result = v[x] + v[y];

    v[x] = result % 256;

    if (result > 255) {
        v[0xF] = 1;
    } else {
        v[0xF] = 0;
    }

}

void cpu::sub_xy(uint8_t x, uint8_t y) {

    uint8_t temp = v[x];
    v[x] -= v[y];
    
    if (temp >= v[y]) {
        v[0xF] = 1;
    } else {
        v[0xF] = 0;
    }


}

void cpu::sub_yx(uint8_t x, uint8_t y) {

    uint8_t temp = v[x];
    v[x] = v[y] - v[x];

    if (v[y] >= temp) {
        v[0xF] = 1;
    } else {
        v[0xF] = 0;
    }


}

void cpu::shift_left(uint8_t x, uint8_t y) {

    v[x] = v[y];
    uint8_t temp = v[x];
    v[x] = temp << 1;
    v[0xF] = (temp & 0x80) >> 7;

}

void cpu:: shift_right(uint8_t x, uint8_t y) {

    v[x] = v[y];
    uint8_t temp = v[x];
    v[x] = temp >> 1;
    v[0xF] = temp & 0x1;

}

void cpu::gen_rand(uint8_t nn, uint8_t x, uint8_t random) {

    v[x] = (random & nn);

}

void cpu::word_index() {

    I = (mem[pc] << 8) | mem[pc + 1];
    pc += 2;

}

void cpu::small_hex(uint8_t x) {

    uint8_t number = v[x] & 0x0F;
    I = number * 5;
}

void cpu::big_hex(uint8_t x) {

    uint8_t number = v[x] & 0x0F;
    I = number * 10 + 80;
}

void cpu::big_skip_check() {

    uint16_t next_opcode = (mem[pc] << 8) | mem[pc + 1];

    if (next_opcode == 0xF000) {
        pc += 2; //additional 2 steps
    }
}

void cpu::BCD(uint8_t x) {

    mem[I] = v[x] / 100;
    mem[I + 1] = (v[x] / 10) % 10;
    mem[I + 2] = v[x] % 10;

}

void cpu::wait(uint8_t x) {

    bool key_pressed = false;
    for (uint8_t i = 0; i < 16; i++) {
        if (keys[i]) {      
            v[x] = i;       
            key_pressed = true;
            return;
        }
        
    }
    if (!key_pressed) {       
        pc -= 2; 
    }
}

void cpu::reg_to_flags(uint8_t x, uint8_t y) {

    for (int i = x; i <= y; i++) {
        flags_storage[I + i] = v[i];
    }

}

void cpu::reg_from_flags(uint8_t x, uint8_t y) {

    for (int i = x; i <= y; i++) {
        v[i] = flags_storage[I + i];
    }
}