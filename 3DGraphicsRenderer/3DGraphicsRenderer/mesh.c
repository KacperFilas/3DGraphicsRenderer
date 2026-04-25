#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>  
#include "mesh.h"
#include "array.h"


mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0,0,0},
    .scale = {1,1,1},
    .translation = {0,0,0}
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y =  1, .z = -1 }, // 2
    { .x =  1, .y =  1, .z = -1 }, // 3
    { .x =  1, .y = -1, .z = -1 }, // 4
    { .x =  1, .y =  1, .z =  1 }, // 5
    { .x =  1, .y = -1, .z =  1 }, // 6
    { .x = -1, .y =  1, .z =  1 }, // 7
    { .x = -1, .y = -1, .z =  1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .a_uv = { 0, 0 }, .b_uv = { 0, 1 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .a_uv = { 0, 0 }, .b_uv = { 1, 1 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF }
};

void load_cube_mesh_data(void) {

    for (int i=0; i < N_CUBE_VERTICES; i++){
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }

    for (int i = 0; i < N_CUBE_FACES; i++)
    {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}

// LOAD OBJ FILE DATA 
void load_obj_file_data(char* filename){
    FILE* file = fopen(filename, "r");
    if (!file) return;
    char line[1024];


    while(fgets(line, sizeof(line), file)){
         // Store the vertices positions in the mesh data
        if(line[0] == 'v' && line[1] ==' '){
            float vertex_position[3];
            
            if(sscanf(
                line, "v %f %f %f", 
                &vertex_position[0], 
                &vertex_position[1], 
                &vertex_position[2]
        
            ) == 3){

                vec3_t vertexPosition = { 
                    .x = vertex_position[0], 
                    .y = vertex_position[1], 
                    .z = vertex_position[2] };

                array_push(mesh.vertices, vertexPosition);
            }
        }

        // store the faces order in the mesh data
        if(line[0] == 'f' && line[1] ==' '){

                int vertices[64], textures[64], normals[64];
                int num_vertices = 0;

                char* skip = line + 2;

                char* token = strtok(skip, " ");

                while (token != NULL){
                    sscanf(token, "%d/%d/%d", 
                        &vertices[num_vertices], 
                        &textures[num_vertices], 
                        &normals[num_vertices]);
                    num_vertices++;
                    token = strtok(NULL, " ");
                }

                // traingle fan algorithm
                for (int i = 1; i < num_vertices - 1; i++) {
                    face_t triangle = {
                            vertices[0],     
                            vertices[i],     
                            vertices[i + 1],
                            .color = 0xFFFFFFFF
                    };

                printf("triangle: %d %d %d\n", triangle.a, triangle.b, triangle.c);
                array_push(mesh.faces, triangle);
                }
        }
    }
  fclose(file);
}


                
