#pragma once

#include <array>

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


int padding = 3;
int titleBarHeight = 24;
Rectangle chip8_screen = {padding, padding, 512, 256 + titleBarHeight};
Rectangle graphics_plane = {padding, chip8_screen.height + (padding * 2), 512, 30};

Vector2 mousePosition = { 0 };
bool dragWindow = false;
Vector2 panOffset = mousePosition;