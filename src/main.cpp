#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "ext/raygui.h"
#include "ext/style_cherry.h"

//include my own other src files
#include "debug.hpp"
#include "cpu.hpp"
#include "graphics.hpp"
#include "sound.hpp"

void handle_sound(cpu& chip8, ma_device& device);
void render_window(graphics& scr);
void handle_game_input();
void render_disassembly_viewport(int x_offset, int y_offset, int view_width, int view_height);
void render_memory_viewport(int x_offset, int y_offset, int view_width, int view_height);
void render_chip8_viewport(graphics& scr, int x_offset, int y_offset, int view_width, int view_height);
const char* get_dissassembly(int offset);

cpu chip8;
audioEngine sound;
audioData audio_data;

int IPF = 11;  //by default
bool speed = false;
bool viewport_info = false;
bool fullscreen = false;
bool run = true;
int window_height = SCREEN_HEIGHT;
int window_width = SCREEN_WIDTH;
std::string rom;
Font customfont;



void init_all() {

    //  raylib/raygui setup
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CHIP8");
    SetTargetFPS(60);
    GuiLoadStyleCherry();  
    GuiEnable();
    customfont = LoadFont("res/fonts/JetBrainsMono-Regular.ttf");


}



int main(int argc, char* argv[]) {

    //one day this will not be necessary...
    if (argc < 2) {
        std::cout << "USAGE: ./chip8 [filename].ch8 \n";
        exit( 1 );
    }   

    graphics scr(chip8.get_memory_ptr(), &chip8);
    chip8.set_graphics_ptr(&scr);

    init_all();

    // miniaudio setup

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_f32;
    config.playback.channels = 2;              
    config.sampleRate        = 48000;  
    config.dataCallback      = sound.data_callback;   // This function will be called when miniaudio needs more data.
    config.pUserData         = &audio_data;   // Can be accessed from the device object (device.pUserData).
    ma_device device;

    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        return -1;  // Failed to initialize the device.
    } 

    // init chip8

    rom = argv[1];
    chip8.init(rom);

    //everything that happens in the window

    ma_device_start(&device);
    
    while (!WindowShouldClose()) {

        handle_game_input();
        handle_window_input();
        handle_sound(chip8, device);
        chip8.tick_timers();

        if (run) {
            for (int i = 0; i < IPF; i++) {
                chip8.execute();
            }
        }
        render_window(scr);  
    }

    ma_device_uninit(&device);

    CloseWindow();
    return 0;
}



void render_window(graphics& scr) {


    mousePosition = GetMousePosition();

    BeginDrawing();

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    if (!fullscreen) {

        if (scr.megachip_mode) {
            chip8_screen.height = MEGACHIP_HEIGHT * 3 + titleBarHeight;
            memory_plane.y = chip8_screen.y + chip8_screen.height + (padding * 5);
            memory_plane.height = SCREEN_HEIGHT - memory_plane_y - 176 + padding;
        }
        render_memory_viewport(memory_plane.x + 1, memory_plane.y + titleBarHeight, memory_plane.width - 2, memory_plane.height);
        render_disassembly_viewport(disassembly.x + 1, disassembly.y + titleBarHeight, disassembly.width - 2, disassembly.height);
        GuiPanel({chip8_screen.x, chip8_screen.y, chip8_screen.width, chip8_screen.height + titleBarHeight + 1}, "Video");
        GuiGroupBox(control_bar, "CONTROL");
        render_chip8_viewport(scr, chip8_screen.x, chip8_screen.y + titleBarHeight, chip8_screen.width, chip8_screen.height);
    } else {
        render_chip8_viewport(scr, 0, 0, window_width, window_height);
    } 


    EndDrawing();
    
}



void render_chip8_viewport(graphics& scr, int x_offset, int y_offset, int view_width, int view_height) {

    BeginScissorMode(x_offset + 1, y_offset, view_width - 2, view_height);
    int curr_height;
    int curr_width;
    int curr_pix_x;
    int curr_pix_y;
    int curr_dim;
    Color curr_color;

    bool* curr_plane1;
    bool* curr_plane2;
    uint8_t* megachip_screen = scr.megachip_scr;

    if (!scr.hires) {
        curr_dim = LORES_DIM;
        curr_height = LORES_HEIGHT;
        curr_width = LORES_WIDTH;
        curr_plane1 = scr.lores_p1;
        curr_plane2 = scr.lores_p2;
    } else {
        curr_dim = HIRES_DIM;
        curr_height = HIRES_HEIGHT;
        curr_width = HIRES_WIDTH;
        curr_plane1 = scr.hires_p1;
        curr_plane2 = scr.hires_p2;
    }

    if (scr.megachip_mode) {
        curr_dim = MEGACHIP_DIM;
        curr_height = MEGACHIP_HEIGHT;
        curr_width = MEGACHIP_WIDTH;
    }

    float scale_x = (float)view_width / curr_width;
    float scale_y = (float)view_height / curr_height;

    for (int i = 0; i < curr_dim; i++) {
        
        if (!scr.megachip_mode) {
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

            uint8_t color_idx = scr.megachip_scrbuffer[i];
            uint32_t color;

            if (color_idx == 0) {
                color = 0;
            } else {
                color = scr.palette[color_idx];
            }

            //unpack color bytes into raylib's RGBA format
            uint8_t alpha = color >> 24;
            uint8_t red = (color >> 16) & 0xFF;
            uint8_t green = (color >> 8) & 0xFF;
            uint8_t blue = color & 0xFF;
            
            curr_color = {red, green, blue, alpha};
        }


        int screen_x = i % curr_width;
        int screen_y = i / curr_width;

        int draw_x_start = x_offset + (int)(screen_x * scale_x);
        int draw_y_start = y_offset + (int)(screen_y * scale_y);
        
        int draw_x_end = x_offset + (int)((screen_x + 1) * scale_x);
        int draw_y_end = y_offset + (int)((screen_y + 1) * scale_y);

        int draw_width = draw_x_end - draw_x_start;
        int draw_height = draw_y_end - draw_y_start;

        DrawRectangle(draw_x_start, 
                      draw_y_start, 
                      draw_width, 
                      draw_height, 
                      curr_color);
    }
    if (viewport_info) {
        DrawRectangle(x_offset, y_offset, 250, 300, viewport_info_bg_color);
        DrawText(TextFormat("IPF = %d", IPF), x_offset, y_offset, 20, YELLOW);
        DrawText(TextFormat("SOUND = %d", chip8.sound), x_offset, y_offset + 20, 20, YELLOW);
    }
    if (audio_data.muted) {
        DrawText("MUTED", view_width - 100, y_offset, 20, RED);
    }
    EndScissorMode();

}



void render_disassembly_viewport(int x_offset, int y_offset, int view_width, int view_height) {

    GuiPanel(disassembly, "Disassembly");

    BeginScissorMode(x_offset, y_offset, view_width, view_height);
    DrawRectangle(x_offset, y_offset, view_width, view_height - (padding * 4 + 1), panel_bg_color);

    for (int i = 0; i < 30 * 2; i += 2) {
        if (i == 0) {
            DrawTextEx(customfont, get_dissassembly(i), (Vector2){(float)x_offset + 5.0f, (float)y_offset + ((i/2) * 25)}, 32, 2, YELLOW);
        } else {
            DrawTextEx(customfont, get_dissassembly(i), (Vector2){(float)x_offset + 5.0f, (float)y_offset + ((i/2) * 25)}, 32, 2, GREEN);
        }
    }
    EndScissorMode();

}

void render_memory_viewport(int x_offset, int y_offset, int view_width, int view_height) {

    GuiPanel(memory_plane, "Memory");
    BeginScissorMode(x_offset, y_offset, view_width, view_height);
    DrawRectangle(x_offset, y_offset, view_width, view_height - (padding * 4 + 1), panel_bg_color);

    for (int i = 0; i < 16; i++) {
        DrawText(TextFormat("V%x = %02x", i, chip8.v[i]), x_offset, y_offset + (i * 18), 20, GREEN);
    }

    EndScissorMode();
}

void handle_window_input() {

    if (IsKeyPressed(KEY_SPACE)) {
        if (!speed) {
            speed = true;
            IPF = 50000;
        } else if (speed) {
            speed = false;
            IPF = 11;
        }
    }

    if (IsKeyPressed(KEY_TAB)) {
        if (!viewport_info) {
            viewport_info = true;
        } else {
            viewport_info = false;
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

    if (IsKeyPressed(KEY_B)) {
        chip8.init(rom);
    }
    
    if (IsKeyPressed(KEY_G)) {
        if (!run) {
            run = true;
        } else {
            run = false;
        }
    }

    if (IsKeyPressed(KEY_M)) {
        if (!audio_data.muted) {
            audio_data.muted = true;
        } else {
            audio_data.muted = false;
        }
    } 
    if (IsKeyPressed(KEY_T)) {
        chip8.execute();
    }
    if (IsKeyPressed(KEY_LEFT_SHIFT)) {
        fullscreen = !fullscreen;
        int monitor_w = GetMonitorWidth(GetCurrentMonitor());
        int monitor_h = GetMonitorHeight(GetCurrentMonitor());
        window_width = monitor_w;
        window_height = monitor_h;
        ToggleFullscreen();
    }
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




const char* get_dissassembly(int offset) {

    static char disassembly[64];

    uint16_t opcode = (chip8.mem[chip8.pc + offset] << 8) | chip8.mem[chip8.pc + 1 + offset];
    uint8_t inst    = (opcode >> 12);
    uint8_t x       = (opcode & 0x0F00) >> 8;
    uint8_t y       = (opcode & 0x00F0) >> 4;
    uint8_t n       = (opcode & 0x000F);
    uint8_t nn      = (opcode & 0x00FF);
    uint16_t nnn    = (opcode & 0x0FFF);
    uint32_t nnnnnn = (nn << 16 & opcode);
    uint32_t byte_3 = chip8.mem[chip8.pc + 2 + offset];
    uint32_t byte_4 = chip8.mem[chip8.pc + 3 + offset];


    int len = std::sprintf(disassembly, "%04X: ", opcode);
    char* mnemonic_ptr = disassembly + len;


    switch (inst) {
        case 0x0:

            switch(x) {
                case 0x1:
                    {
                    uint32_t nnnnnn = (nn << 16) | (byte_3 << 8) | byte_4;
                    std::sprintf(mnemonic_ptr, "LD I, 0x%06X", nnnnnn);
                    break;
                    }
                case 0x2: std::sprintf(mnemonic_ptr, "LD PALETTE, %02X", nn); break;
                case 0x3: std::sprintf(mnemonic_ptr, "SPRW := %02X", nn); break;
                case 0x4: std::sprintf(mnemonic_ptr, "SPRH := %02X", nn); break;
                case 0x5: std::sprintf(mnemonic_ptr, "ALPHA := %02X", nn); break;
                case 0x6: std::sprintf(mnemonic_ptr, "PLAYSOUND, LP := %X", n); break;
                case 0x7: std::sprintf(mnemonic_ptr, "STOPSOUND"); break;
                case 0x8: std::sprintf(mnemonic_ptr, "BLEND := %X", n); break;
                case 0x9: std::sprintf(mnemonic_ptr, "COLL COLOR := %02X", nn); break;
            }
            if (nnn == 0x0E0) {
                std::sprintf(mnemonic_ptr, "CLS");
            } else if (nnn == 0x0EE) {

                std::sprintf(mnemonic_ptr, "RET");
            }
            break;
        case 0x1: std::sprintf(mnemonic_ptr, "JP 0x%03X", nnn); break;
        case 0x2: std::sprintf(mnemonic_ptr, "CALL 0x%03X", nnn); break;
        case 0x3: std::sprintf(mnemonic_ptr, "SE V%x, 0x%02X", x, nn); break;
        case 0x4: std::sprintf(mnemonic_ptr, "SNE V%x, 0x%02X", x, nn); break;

        case 0x5: 
            switch(n) {
                case 0x0: std::sprintf(mnemonic_ptr, "SE V%x, v%x", x, y); break;
                case 0x2: std::sprintf(mnemonic_ptr, "STORE V%x, v%x", x, y); break;
                case 0x3: std::sprintf(mnemonic_ptr, "GET V%x, v%x", x, y); break;
            }
            break;

        case 0x6: std::sprintf(mnemonic_ptr, "LD V%x, 0x%02X", x, nn); break;
        case 0x7: std::sprintf(mnemonic_ptr, "V%x += %02X", x, nn); break;

        case 0x8:
            switch (n) {
                case 0x0: std::sprintf(mnemonic_ptr, "V%x := V%x", x, y); break;
                case 0x1: std::sprintf(mnemonic_ptr, "V%x | V%x", x, y); break;
                case 0x2: std::sprintf(mnemonic_ptr, "V%x & V%x", x, y); break;
                case 0x3: std::sprintf(mnemonic_ptr, "V%x ^ V%x", x, y); break;
                case 0x4: std::sprintf(mnemonic_ptr, "V%x + V%x", x, y); break;
                case 0x5: std::sprintf(mnemonic_ptr, "V%x - V%x", x, y); break;
                case 0x6: std::sprintf(mnemonic_ptr, "V%x >> 1", x); break;
                case 0x7: std::sprintf(mnemonic_ptr, "V%x - V%x", y, x); break;
                case 0xE: std::sprintf(mnemonic_ptr, "V%x << 1", x); break;

                default:
                    std::sprintf(mnemonic_ptr, "UNKNOWN (0x%04X)", opcode);
            }
            break;

        case 0x9: std::sprintf(mnemonic_ptr, "SNE V%x,  V%x", x, y); break;
        case 0xA: std::sprintf(mnemonic_ptr, "LD I, 0x%03X", nnn); break;
        case 0xB: std::sprintf(mnemonic_ptr, "PC := %03X + v0", nn); break;
        case 0xC: std::sprintf(mnemonic_ptr, "RAND & V%x", x); break;
        case 0xD: std::sprintf(mnemonic_ptr, "DRAW V%X, V%X, %d", x, y, n); break;
        case 0xE:
            if (nn == 0x9E) {
                std::sprintf(mnemonic_ptr, "SKIP PRSD V%x", x);
            } else if (nn == 0xA1) {
                std::sprintf(mnemonic_ptr, "SKIP N PRSD V%x", x);
            }
            break;
        case 0xF:
            switch(nn) {
                case 0x00: 
                    {
                    uint16_t nnnn = (byte_3 << 8) | byte_4;
                    std::sprintf(mnemonic_ptr, "LD I, 0x%04X", nnnn);
                    break;
                    }
                
                case 0x01: std::sprintf(mnemonic_ptr, "PLANE %x", x); break;
                case 0x02: std::sprintf(mnemonic_ptr, "LD AUDIO PATTERN"); break;
                case 0x07: std::sprintf(mnemonic_ptr, "V%x := DELAY", x); break;
                case 0x15: std::sprintf(mnemonic_ptr, "DELAY := V%x", x); break;
                case 0x18: std::sprintf(mnemonic_ptr, "SOUND := V%x", x); break;
                case 0x1E: std::sprintf(mnemonic_ptr, "I += V%x", x); break;
                case 0x29: std::sprintf(mnemonic_ptr, "SMALLHEX V%x", x); break;
                case 0x30: std::sprintf(mnemonic_ptr, "BIGHEX V%x", x); break;
                case 0x33: std::sprintf(mnemonic_ptr, "BCD"); break;
                case 0x55: std::sprintf(mnemonic_ptr, "WRITE V0-V%x", x); break;
                case 0x65: std::sprintf(mnemonic_ptr, "READ V0-V%x", x); break;
            } 
            break;
        default:
            std::sprintf(mnemonic_ptr, "UNKNOWN (0x%04X)", opcode);
            break;
    }

    return disassembly;
}

void handle_sound(cpu& chip8, ma_device& device) {

    for (int i = 0; i < 16; i++) {
        audio_data.audioBuffer[i] = chip8.audioBuffer[i];
    }

    audio_data.targetFrequency = chip8.frequency;

    if (chip8.sound > 0) {
        audio_data.sound_timer_active = true;
    } else {
        audio_data.sound_timer_active = false;
    }
}