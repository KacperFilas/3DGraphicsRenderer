#include "light.h"


light_t main_light = {
    .direction = { 0, 0, 1 }
};

uint32_t light_apply_intensity(uint32_t original_color, float perecentage_factor){

    if(perecentage_factor < 0 ) perecentage_factor = 0;
    if(perecentage_factor > 1) perecentage_factor = 1;

    uint32_t a = (original_color & 0xFF000000);
    uint32_t r = (original_color & 0x00FF0000) * perecentage_factor;
    uint32_t g = (original_color & 0x0000FF00) * perecentage_factor;
    uint32_t b = (original_color & 0x000000FF) * perecentage_factor;

    uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);

    return new_color;
}


