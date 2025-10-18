#pragma once

const int LORES_DIM = 64 * 32;
const int HIRES_DIM = 128 * 64;

class graphics {
    public:

        //bitplanes

        //low res (lores)
        bool lores_p1[LORES_DIM];
        bool lores_p2[LORES_DIM];

        //high res (hires) 
        bool hires_p1[HIRES_DIM];
        bool hires_p2[HIRES_DIM];

        bool lores;

        bool draw_sprite(uint8_t x, uint8_t y, uint8_t height);
};