#include <math.h>
#include "vector.h"


// Properties of the cos and sin functions
// x = r * cos(alfa)
// y = r * sin(alfa)

// Final rotation matrix equations derived from the properties of the cos and sin functions
// x' = r * cos(alfa + beta)
// x' = r * (cos(alfa) * cos(beta) - sin(alfa) * sin(beta)) -- angle addition law
// x' = x * cos(beta) - y * sin(beta)

// y' = r * sin(alfa + beta)
// y' = r * (sin(alfa) * cos(beta) + cos(alfa) * sin(beta)) -- angle addition law
// y' = x * sin(beta) + y * cos(beta)

// Matrix form (standard for 2D graphics):
// [ x' ]   [ cos(beta)  -sin(beta) ] [ x ]
// [ y' ] = [ sin(beta)   cos(beta) ] [ y ]

// Note:
// 1. Rotation is counter-clockwise (mathematical standard).
// 2. Beta must be in radians: rad = deg * (PI / 180).
// 3. In screen coordinates (Y-down), the visual rotation might appear inverted.

vec3_t vec3_rotate_x(vec3_t v, float angle){
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };

    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle){
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) + v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) - v.z * cos(angle)
    };

    return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle){
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };

    return rotated_vector;
}
