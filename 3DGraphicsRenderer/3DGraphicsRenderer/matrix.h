#ifndef MATRIX_H
#define MATRIX_H
#include "vector.h"


typedef struct {
    float m[4][4];
} matrix4_t;

matrix4_t matrix4_identity(void);
matrix4_t matrix4_make_scale(float sx, float sy, float sz);
matrix4_t matrix4_make_translation(float tx, float ty, float tz);
matrix4_t matrix4_make_rotation_x(float angle);
matrix4_t matrix4_make_rotation_y(float angle);
matrix4_t matrix4_make_rotation_z(float angle);
vec4_t matrix4_mul_vec4(matrix4_t m, vec4_t v);
matrix4_t matrix4_mul_matrix4(matrix4_t a, matrix4_t b);

#endif