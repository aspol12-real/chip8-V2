#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "ext/raygui.h"
#include "ext/style_cherry.h"

//include my own other src files
#include "debug.hpp"
#include "cpu.hpp"
#include "graphics.hpp"
#include "sound.hpp"

cpu chip8;

int IPF = 11;  
bool speed = false;
bool viewport_info = false;
bool fullscreen = false;
int window_height = SCREEN_HEIGHT;
int window_width = SCREEN_WIDTH;

void render_window();
void handle_game_input();



int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "USAGE: ./chip8 [filename].ch8 \n";
        exit( 1 );
    }   

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CHIP8");
    SetTargetFPS(60);
    GuiLoadStyleCherry();  
    GuiEnable();

    chip8.init();

    std::string rom = argv[1];
    chip8.load_rom(rom);

    //everything that happens in the window



    while (!WindowShouldClose()) {

        handle_game_input();
        handle_window_input();
        chip8.tick_timers();

        for (int i = 0; i < IPF; i++) {
            chip8.execute();
        }

        render_window(); //render @60 fps

    }

    CloseWindow();
    return 0;
}

void handle_game_input() {

    chip8.keys[0x1] = IsKeyDown(KEY_ONE);
    chip8.keys[0x2] = IsKeyDown(KEY_TWO);
    chip8.keys[0x3] = IsKeyDown(KEY_THREE);
    chip8.keys[0xC] = IsKeyDown(KEY_FOUR);
    chip8.keys[0x4] = IsKeyDown(KEY_Q);
    chip8.keys[0x5] = IsKeyDown(KEY_W);
    chip8.keys[0x6] = IsKeyDown(KEY_E);
    chip8.keys[0xD] = IsKeyDown(KEY_R);
    chip8.keys[0x7] = IsKeyDown(KEY_A);
    chip8.keys[0x8] = IsKeyDown(KEY_S);
    chip8.keys[0x9] = IsKeyDown(KEY_D);
    chip8.keys[0xE] = IsKeyDown(KEY_F);
    chip8.keys[0xA] = IsKeyDown(KEY_Z);
    chip8.keys[0x0] = IsKeyDown(KEY_X);
    chip8.keys[0xB] = IsKeyDown(KEY_C);
    chip8.keys[0xF] = IsKeyDown(KEY_V);

} 

void render_window() {


    mousePosition = GetMousePosition();

    update_window_dimensions(fullscreen);

    BeginDrawing();
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    if (!fullscreen) {
        GuiPanel(graphics_plane, "Graphics");
        GuiGroupBox(tabs, "CONTROLS");
        if (chip8.scr.megachip_mode) {
            chip8_screen.height = MEGACHIP_HEIGHT * 2 + titleBarHeight;
        }
        render_chip8_viewport(chip8_screen.x, chip8_screen.y + titleBarHeight, chip8_screen.width, chip8_screen.height);
    } else {

        int monitor_width = GetMonitorWidth(GetCurrentMonitor());
        int monitor_height = GetMonitorHeight(GetCurrentMonitor());     

        render_chip8_viewport(0, 0, window_width, window_height);
    } 


    EndDrawing();
    
}

void render_chip8_viewport(int x_offset, int y_offset, int view_width, int view_height) {

    if (!fullscreen) {
        GuiPanel(chip8_screen, "CHIP-8");
    }


    int curr_height;
    int curr_width;
    int curr_pix_x;
    int curr_pix_y;
    int curr_dim;
    Color curr_color;

    bool* curr_plane1;
    bool* curr_plane2;
    uint8_t* megachip_screen = chip8.scr.megachip_scr;

    if (!chip8.scr.hires) {
        curr_dim = LORES_DIM;
        curr_height = LORES_HEIGHT;
        curr_width = LORES_WIDTH;
        curr_plane1 = chip8.scr.lores_p1;
        curr_plane2 = chip8.scr.lores_p2;
    } else {
        curr_dim = HIRES_DIM;
        curr_height = HIRES_HEIGHT;
        curr_width = HIRES_WIDTH;
        curr_plane1 = chip8.scr.hires_p1;
        curr_plane2 = chip8.scr.hires_p2;
    }

    if (chip8.scr.megachip_mode) {
        curr_dim = MEGACHIP_DIM;
        curr_height = MEGACHIP_HEIGHT;
        curr_width = MEGACHIP_WIDTH;
    }

    curr_pix_x = view_width / curr_width;
    curr_pix_y = view_height / curr_height;

    for (int i = 0; i < curr_dim; i++) {
        
        if (!chip8.scr.megachip_mode) {
            uint8_t p1 = curr_plane1[i];
            uint8_t p2 = curr_plane2[i];

            if (p1 && p2) {
                curr_color = current_Pallete[0];
            }
            else if (p1 && (!p2)) {
                curr_color = current_Pallete[1];
            }
            else if ((!p1) && p2) {
                curr_color = current_Pallete[2];
            }
            else if (!(p1 || p2)) {
                curr_color = current_Pallete[3];
            }
        } else { //megachip rendering enabled

            //ARGB FORMAT

            uint8_t color_idx = chip8.scr.megachip_scr[i];
            
            uint32_t color = (color_idx < MEGACHIP_COLORS) ? chip8.scr.palette[color_idx] : 0xFF000000;

            uint8_t alpha = color >> 24;
            uint8_t red = (color >> 16) & 0xFF;
            uint8_t green = (color >> 8) & 0xFF;
            uint8_t blue = color & 0xFF;
            
            curr_color = {red, green, blue, alpha};
        }
        DrawRectangle(x_offset + ((i % curr_width) * curr_pix_x), 
                          y_offset + ((i / curr_width) * curr_pix_y), 
                          curr_pix_x, curr_pix_y, curr_color);
    }

}

void handle_window_input() {

    if (IsKeyPressed(KEY_SPACE)) {
        if (speed == false) {
            speed = true;
            IPF = 2000;
        } else if (speed == true) {
            speed = false;
            IPF = 11;
        }
    }
    if (IsKeyPressed(KEY_FIVE)) {
        current_Pallete = palette1;
    }
    if (IsKeyPressed(KEY_SIX)) {
        current_Pallete = palette2;
    }
    if (IsKeyPressed(KEY_SEVEN)) {
        current_Pallete = palette3;
    }
    if (IsKeyPressed(KEY_EIGHT)) {
        current_Pallete = palette4;
    }

    if (IsKeyPressed(KEY_TAB)) {
        if (!viewport_info) {
            viewport_info = true;
        } else {
            viewport_info = false;
        }
    }

    if (IsKeyPressed(KEY_LEFT_SHIFT)) {
        fullscreen = !fullscreen;
        ToggleFullscreen();
    }
}

void update_window_dimensions(bool fullscreen) {
    

    if (IsWindowResized()) {
        

        int new_width = GetScreenWidth();
        int new_height = GetScreenHeight();

        if (!fullscreen) {

            /*
            graphics_plane.width = new_width - (padding * 2);

            int graphics_plane_y = chip8_screen.y + chip8_screen.height + padding;
            int graphics_plane_height = SCREEN_HEIGHT - graphics_plane_y - padding;

            graphics_plane.height = graphics_plane_height;
            */

        }
    }
}
