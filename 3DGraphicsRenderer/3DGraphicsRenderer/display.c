#include "display.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void){
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n ", SDL_GetError());
        return false;
    }

    /// Use SDL to query what is the fullscreen max width and height

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h; 

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
     );
     
     if(!window){
         fprintf(stderr, "Could not create window! SDL_Error: %s\n ", SDL_GetError());
         return false;
     }

     // Create a renderer for the window
     renderer = SDL_CreateRenderer(window, -1, 0);

     if(!renderer){
         fprintf(stderr, "Could not create renderer! SDL_Error: %s\n", SDL_GetError());
         return false;
     }


    return true;

}

void render_color_buffer(void){
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t))
    );

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);

}


void clear_color_buffer(uint32_t color){
    for (int y = 0; y < window_height; y++){
        for (int x = 0; x < window_width; x++){
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void draw_grid(void){
    // Drawing a grid on the color buffer
    for (int y = 0; y < window_height; y++){
        for (int x = 0; x < window_width; x++){
            if( y % 10 == 0 || x % 10 == 0 || y == window_height - 1 || x == window_width - 1){ 
                color_buffer[(window_width * y) + x] = 0xFF0000FF;
            }
        }
    } 
}

void draw_pixel(int x, int y, uint32_t color){
    if(x >= 0 && x < window_width && y >= 0 && y < window_height){
        color_buffer[(window_width * y) + x] = color;
    }
}


// Draw a line  DDA
void draw_line(int x0, int y0, int x1, int y1, uint32_t color){
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int longest_side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)longest_side_length;
    float y_inc = delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++){
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}


// Draw a rectangle on the color buffer
void draw_rectangle(int x, int y, int width, int height, uint32_t color){
    for (int current_y = y; current_y < y + height; current_y++){
        for (int current_x = x; current_x < x + width; current_x++){
            draw_pixel(current_x, current_y, color);
        }
    }
}


void destroy_window(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}



