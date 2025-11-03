#pragma once

#include <cstdint>
#include <iostream>
#include <string.h>

const int LORES_WIDTH = 64;
const int LORES_HEIGHT = 32;
const int LORES_DIM = LORES_WIDTH * LORES_HEIGHT;

const int HIRES_WIDTH = 128;
const int HIRES_HEIGHT = 64;
const int HIRES_DIM = HIRES_WIDTH * HIRES_HEIGHT;

const int MEGACHIP_WIDTH = 256;
const int MEGACHIP_HEIGHT = 192;
const int MEGACHIP_DIM = MEGACHIP_WIDTH * MEGACHIP_HEIGHT;

const int MEGACHIP_COLORS = 255;

class cpu;

class graphics {
    private:

        uint8_t* m_mem_ptr;
        cpu* m_cpu_ptr;


        bool collision(int offset);
        void pix_to_plane(int offset);
        bool get_pixel(int offset);


        //megachip 
        bool draw_sprite_megachip(uint8_t x, uint8_t y, uint8_t height, uint16_t I);
    public:

        graphics() {}

        graphics(uint8_t* mem_ptr, cpu* cpu_ptr) 
        : m_mem_ptr(mem_ptr), m_cpu_ptr(cpu_ptr) {}

        //bitplanes

        //low res (lores)
        bool lores_p1[LORES_DIM];
        bool lores_p2[LORES_DIM];

        //high res (hires) 
        bool hires_p1[HIRES_DIM];
        bool hires_p2[HIRES_DIM];

        //megachip
        uint8_t megachip_scr[MEGACHIP_DIM];
        uint32_t palette[MEGACHIP_COLORS];
        uint8_t sprw;
        uint8_t sprh;
        uint8_t alpha;

        uint8_t b_mode;

        bool draw_flag = false;
        bool megachip_mode;
        bool hires;
        uint8_t selected_plane;

        void clear_all();
        void clear_plane();

        void scroll_left();
        void scroll_right();
        void scroll_down(uint8_t n);
        void scroll_up_schip(uint8_t n);

        bool draw_sprite8(uint8_t x, uint8_t y, uint8_t height, uint16_t I);
        bool draw_sprite16(uint8_t x, uint8_t y, uint16_t I);
};