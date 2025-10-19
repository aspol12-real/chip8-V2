#pragma once

#include <cstdint>
#include <iostream>

const int LORES_WIDTH = 64;
const int LORES_HEIGHT = 32;
const int LORES_DIM = LORES_WIDTH * LORES_HEIGHT;

const int HIRES_WIDTH = 128;
const int HIRES_HEIGHT = 64;
const int HIRES_DIM = HIRES_WIDTH * HIRES_HEIGHT;

class graphics {
    private:

        uint8_t* m_mem_ptr;
    public:

        graphics(uint8_t* mem_ptr) : m_mem_ptr(mem_ptr) {}


        //bitplanes

        //low res (lores)
        bool lores_p1[LORES_DIM];
        bool lores_p2[LORES_DIM];

        //high res (hires) 
        bool hires_p1[HIRES_DIM];
        bool hires_p2[HIRES_DIM];

        bool hires;
        uint8_t selected_plane;

        bool draw_sprite8(uint8_t x, uint8_t y, uint8_t height, uint16_t I);
        bool draw_sprite16(uint8_t x, uint8_t y, uint16_t I);

        bool collision(int offset);
        void pix_to_plane(int offset);

        void clear_all();
        void clear_plane();
};