#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

bool is_running = false;

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

    /////// Use SDL to query what is the fullscreen max width and height

    // SDL_DisplayMode display_mode;
    // SDL_GetCurrentDisplayMode(0, &display_mode);
    // window_width = display_mode.w;
    // window_height = display_mode.h; 

    // Create a SDL window
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

    /////  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;

}



void setup(void){
    // Initialize the color buffer 
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);

    // Create a texture for the color buffer 
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
}

void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type){
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE){
                is_running = false;
            }
            break;
    }
}

void update(void){
    //todo
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

// Draw a rectangle on the color buffer
void draw_rectangle(int x, int y, int width, int height, uint32_t color){
    for (int i = y ; i < y + height; i++){
        for (int j = x; j < x + width; j++){
            color_buffer[(window_width * i) + j ] = color;
        }
    }
}


void render(void){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    draw_grid();
    draw_rectangle(300, 200, 300, 150, 0xFFFFFFFF);
    render_color_buffer();
    clear_color_buffer(0xFF000000);
    
    SDL_RenderPresent(renderer);
    
}

void destroy_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main(void){

    is_running = initialize_window();

    setup();

    while(is_running){
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}  