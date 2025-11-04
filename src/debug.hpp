#pragma once

#include <array>
#include <raylib.h>

int SCREEN_HEIGHT = 800;
int SCREEN_WIDTH = 1280;

int chip8_width = 800;
int chip8_height = 400;

std::array<Color, 4> palette1 = {{
    {244, 233, 205, 255},
    {119, 172, 62, 255},
    {70, 129, 137, 255},
    {3, 25, 38, 255}
}};

std::array<Color, 4> palette2 = {{
    {191, 234, 195, 255},
    {112, 151, 117, 255},
    {65, 93, 67, 255},
    {17, 29, 19, 255}
}};

std::array<Color, 4> palette3 = {{
    {255, 240, 243, 255}, 
    {255, 77, 109, 255},  
    {164, 19, 60, 255},   
    {89, 13, 34, 255}     
}};

std::array<Color, 4> palette4 = {{
    {255, 255, 255, 255},
    {175, 175, 175, 255},
    {65, 65, 65, 255},
    {0, 0, 0, 255}
}};

std::array<Color, 4> current_Pallete = palette1;

void handle_window_input();


int padding = 6;
float titleBarHeight = 24;

Rectangle chip8_screen = {
    (float)padding, 
    (float)padding, 
    (float)chip8_width, 
    (float)chip8_height + (titleBarHeight + (padding * 2))
};

int disassembly_x = chip8_screen.x + chip8_screen.width + padding;
int disassembly_width = SCREEN_WIDTH - disassembly_x - padding - 90;
int disassembly_height = SCREEN_HEIGHT - (padding * 2);

Rectangle disassembly = {
    (float)disassembly_x,
    (float)padding, 
    (float)disassembly_width,
    (float)disassembly_height
};

int control_bar_x = disassembly_x + disassembly_width + padding;
int control_bar_width = SCREEN_WIDTH - control_bar_x - padding;
int control_bar_height = SCREEN_HEIGHT - (padding * 2);

Rectangle control_bar = {
    (float)control_bar_x,
    (float)padding,
    (float)control_bar_width,
    (float)control_bar_height
};

int memory_plane_y = chip8_screen.y + chip8_screen.height + (padding * 5) ;
int memory_plane_height = SCREEN_HEIGHT - memory_plane_y - padding;
int memory_plane_width = chip8_screen.width;


Rectangle memory_plane = {
    (float)padding, 
    (float)memory_plane_y, 
    (float)memory_plane_width,
    (float)memory_plane_height
};

Vector2 mousePosition = { 0 };

Color panel_bg_color = {19, 6, 6, 255};
Color viewport_info_bg_color = {19, 19, 19, 190};