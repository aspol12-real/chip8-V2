#include "graphics.hpp"

void graphics::clear_all() {

    for (int i = 0; i < LORES_DIM; i++) {
        lores_p1[i] = false;
        lores_p2[i] = false;
    }

    for (int i = 0; i < HIRES_DIM; i++) {
        hires_p1[i] = false;
        hires_p2[i] = false;
    }

}

void graphics::clear_plane() {

    if(!hires) {
        for (int i = 0; i < LORES_DIM; i++) {
            if (selected_plane == 1) {
                lores_p1[i] = false;
            } else if (selected_plane == 2) {
                lores_p2[i] = false;
            } else if (selected_plane == 3) {
                lores_p1[i] = false;
                lores_p2[i] = false;
            }
        }
    } else {
       for (int i = 0; i < HIRES_DIM; i++) {
            if (selected_plane == 1) {
                hires_p1[i] = false;
            } else if (selected_plane == 2) {
                hires_p2[i] = false;
            } else if (selected_plane == 3) {
                hires_p1[i] = false;
                hires_p2[i] = false;
            }
        } 
    }

}

bool graphics::draw_sprite8(uint8_t x, uint8_t y, uint8_t height, uint16_t I) {

    if (megachip_mode) {
        return draw_sprite_megachip(x, y, height, I);
    }

    int curr_height;
    int curr_width;

    bool collision_flag = false;
    if (!hires) {
        curr_height = LORES_HEIGHT;
        curr_width = LORES_WIDTH;
    } else {
        curr_height = HIRES_HEIGHT;
        curr_width = HIRES_WIDTH;
    }

    for (int i = 0; i < height; i++) {

        uint8_t spriteByte = m_mem_ptr[I + i];

        for (int j = 0; j < 8; j++) {

            uint8_t bit = (spriteByte >> (7 - j)) & 0x1;

            if (bit != 0) {

                uint8_t scr_y = (y + i) % curr_height;
                uint8_t scr_x = (x + j) % curr_width;

                int offset = (scr_y * curr_width) + scr_x;

                bool before = get_pixel(offset);

                pix_to_plane(offset);

                bool after = get_pixel(offset);

                if (before && !after) {
                    collision_flag = true;
                }
            }
        }
    }
    return collision_flag;
}

bool graphics::draw_sprite16(uint8_t x, uint8_t y, uint16_t I) {

    if (megachip_mode) {
        return false;
    }

    bool collision_flag = false;
    
    int curr_height;
    int curr_width;

    if (!hires) {
        curr_height = LORES_HEIGHT;
        curr_width = LORES_WIDTH;
    } else {
        curr_height = HIRES_HEIGHT;
        curr_width = HIRES_WIDTH;
    }

    for (int i = 0; i < 16; i++) {

        uint8_t spriteByte1 = m_mem_ptr[I + i * 2];
        uint8_t spriteByte2 = m_mem_ptr[I + i * 2 + 1];

        uint16_t slice = (spriteByte1 << 8) | spriteByte2;

        for (int j = 0; j < 16; j++) {

            uint8_t bit = (slice >> (15 - j)) & 0x1;

            if (bit != 0) {

                uint8_t scr_y = (y + i) % curr_height;
                uint8_t scr_x = (x + j) % curr_width;

                int offset = (scr_y * curr_width) + scr_x;

                bool before = get_pixel(offset);

                pix_to_plane(offset);

                bool after = get_pixel(offset);

                if (before && !after) {
                    collision_flag = true;
                }

            }
        }
    }
    return collision_flag;

}



bool graphics::collision(int offset) {

    if (!hires) {
        if (selected_plane == 1) {
            if(lores_p1[offset]) {
                return true;
            }
        } else if (selected_plane == 2) {
            if(lores_p2[offset]) {
                return true;
            }
        } else if (selected_plane == 3) {
            if(lores_p1[offset] && lores_p2[offset]) {
                return true;
            }
        }
    } else {
        if (selected_plane == 1) {
            if(hires_p1[offset]) {
                return true;
            }
        } else if (selected_plane == 2) {
            if(hires_p2[offset]) {
                return true;
            }
        } else if (selected_plane == 3) {
            if(hires_p1[offset] && hires_p2[offset]) {
                return true;
            }
        }
    }
    return false;
}

void graphics::pix_to_plane(int offset) {

    if (!hires) {

        if (selected_plane == 1) {
            lores_p1[offset] ^= true;
        } else if (selected_plane == 2) {
            lores_p2[offset] ^= true;
        } else if (selected_plane == 3) {
            lores_p1[offset] ^= true;
            lores_p2[offset] ^= true;
        }

    } else {

        if (selected_plane == 1) {
            hires_p1[offset] ^= true;
        } else if (selected_plane == 2) {
            hires_p2[offset] ^= true;
        } else if (selected_plane == 3) {
            hires_p1[offset] ^= true;
            hires_p2[offset] ^= true;
        }
    }
}

bool graphics::get_pixel(int offset) {
    if (!hires) {
        if (selected_plane == 1) return lores_p1[offset];
        if (selected_plane == 2) return lores_p2[offset];
        if (selected_plane == 3) return lores_p1[offset] || lores_p2[offset];
    } else {
        if (selected_plane == 1) return hires_p1[offset];
        if (selected_plane == 2) return hires_p2[offset];
        if (selected_plane == 3) return hires_p1[offset] || hires_p2[offset];
    }
    return false;
}

void graphics::scroll_down(uint8_t n) {
    
}

void graphics::scroll_up_schip(uint8_t n) {

}

bool graphics::draw_sprite_megachip(uint8_t x, uint8_t y, uint8_t height, uint16_t I) {



    bool collision_flag = false;
    for (int i = 0; i < sprh; i++) {

        int sprite_row_offset = i * sprw;
        uint8_t scr_y = (y + i) % MEGACHIP_HEIGHT;

        for (int j = 0; j < sprw; j++) {

            uint8_t scr_x = (x + j) % MEGACHIP_WIDTH;
            int offset = (scr_y * MEGACHIP_WIDTH) + scr_x;

            uint8_t spritePixel = m_mem_ptr[I + sprite_row_offset + j];

            uint8_t background_color = megachip_scr[offset];
            if (spritePixel != 0) {
                if (background_color > 0) {
                    collision_flag = true;
                }

                megachip_scr[offset] = spritePixel;
            }
        }
    }

    return collision_flag;
}