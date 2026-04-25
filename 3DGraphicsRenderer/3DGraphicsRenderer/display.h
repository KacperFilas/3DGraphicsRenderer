#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>


#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS) 


extern SDL_Window* window;
extern SDL_Renderer* renderer;

extern uint32_t* color_buffer;
extern SDL_Texture* color_buffer_texture;

extern int window_width;
extern int window_height; 

typedef enum {
    RENDER_WIREFRAME_VERTEX,
    RENDER_WIREFRAME,
    RENDER_FILLED,
    RENDER_FILLED_WIREFRAME,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIREFRAME
} render_mode;

extern render_mode current_render_mode;

bool initialize_window(void);



void draw_grid(void);

void draw_pixel(int x, int y, uint32_t color);

void draw_line(int x0, int y0, int x1, int y1, uint32_t color);

void draw_rectangle(int x, int y, int width, int height, uint32_t color);

void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void destroy_window(void);

#endif // DISPLAY_H