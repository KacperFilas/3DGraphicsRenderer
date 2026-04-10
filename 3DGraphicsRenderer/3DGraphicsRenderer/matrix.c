#include "matrix.h"
#include <math.h>

matrix4_t matrix4_identity(void){
   // | 1 0 0 0 |
   // | 0 1 0 0 |
   // | 0 0 1 0 |
   // | 0 0 0 1 |
    matrix4_t m = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};
    return m;
}

matrix4_t matrix4_make_scale(float sx, float sy, float sz){
    // | sx 0 0 0 |
    // | 0 sy 0 0 |
    // | 0 0 sz 0 |
    // | 0 0 0  1 |
    matrix4_t m = matrix4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
}

matrix4_t matrix4_make_translation(float tx, float ty, float tz){
    // | 1 0 0 tx |
    // | 0 1 0 ty |
    // | 0 0 1 tz |
    // | 0 0 0  1 |
    matrix4_t m = matrix4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}

matrix4_t matrix4_make_rotation_x(float angle){
    // | 1 0 0 0 |
    // | 0 c -s 0 |
    // | 0 s c 0 |
    // | 0 0 0 1 |
    matrix4_t m = matrix4_identity();
    float c = cos(angle);
    float s = sin(angle);
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}

matrix4_t matrix4_make_rotation_y(float angle){
    // | c 0 s 0 |
    // | 0 1 0 0 |
    // | -s 0 c 0 |
    // | 0 0 0 1 |
    matrix4_t m = matrix4_identity();
    float c = cos(angle);
    float s = sin(angle);
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
}

matrix4_t matrix4_make_rotation_z(float angle){
    // | c -s 0 0 |
    // | s c 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |
    matrix4_t m = matrix4_identity();
    float c = cos(angle);
    float s = sin(angle);
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m;
}

vec4_t matrix4_mul_vec4(matrix4_t m, vec4_t v){
    vec4_t result;
        result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
        result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
        result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
        result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}


matrix4_t matrix4_mul_matrix4(matrix4_t a, matrix4_t b){
    matrix4_t result;
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }
    return result;
}

matrix4_t matrix4_make_perspective(float fov, float aspect, float znear, float zfar){
     

    matrix4_t m = {{{0}}};
    m.m[0][0] = - ((1 / tan(fov / 2))/ aspect);
    m.m[1][1] = 1 / tan(fov / 2);
    m.m[2][2] = zfar / (zfar - znear);
    m.m[2][3] = (-zfar * znear) / (zfar - znear);
    m.m[3][2] = 1.0;

    return m;
}

vec4_t matrix4_mul_vec4_project(matrix4_t mat_proj, vec4_t v){
    // Multiply the vector by the projection matrix

    vec4_t result = matrix4_mul_vec4(mat_proj, v);
    // perform perspective division with original z value that is now in w
    if(result.w != 0){
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    
    return result; 
}
    


