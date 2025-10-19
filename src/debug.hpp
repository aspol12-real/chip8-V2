#pragma once

#include <array>
#include <raylib.h>

int SCREEN_HEIGHT = 600;
int SCREEN_WIDTH = 960;

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

//function declarations!

void render_chip8_viewport(int x_offset, int y_offset, int view_width, int view_height);
void handle_window_input();
void update_window_dimensions(bool fullscreen);;


int padding = 17;
float titleBarHeight = 24;

Rectangle chip8_screen = {(float)padding, (float)padding, 512, 256 + titleBarHeight};


int graphics_plane_y = chip8_screen.y + chip8_screen.height + padding;
int graphics_plane_height = SCREEN_HEIGHT - graphics_plane_y - padding;

Rectangle graphics_plane = {
    (float)padding, 
    (float)graphics_plane_y, 
    512,
    (float)graphics_plane_height
};


int tabs_x = chip8_screen.x + chip8_screen.width + padding;
int tabs_width = SCREEN_WIDTH - tabs_x - padding;

Rectangle tabs = {
    (float)tabs_x,
    (float)padding, 
    (float)tabs_width,
    40 
};

Vector2 mousePosition = { 0 };
bool dragWindow = false;
Vector2 panOffset = mousePosition;