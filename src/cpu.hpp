#pragma once

#include <cstdint>

class cpu {
    private:

        uint8_t mem[65535]; //65kb memory space
        uint8_t v[16]; //8-bit registers v0-vF (16 of them)

        uint16_t pc; //16-bit program counter
        uint16_t sp; //16-bit stack pointer
        uint16_t I;  //16-bit memory pointer

        uint8_t delay; //8-bit delay timer

    public:

        uint8_t sound; //8-bit sound timer

        void init();
        void execute();
         fetch_sprite(uint16_t index, uint8_t height);
};