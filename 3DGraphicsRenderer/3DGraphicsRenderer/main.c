#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include "display.h"
#include "array.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"

// Array of triangles to render
triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { 0, 0, 0 };

enum render_mode {
    RENDER_WIREFRAME_VERTEX,
    RENDER_WIREFRAME,
    RENDER_FILLED,
    RENDER_FILLED_WIREFRAME,
} render_mode;

enum render_mode current_render_mode = RENDER_FILLED_WIREFRAME;
bool back_face_culling = true;
     
float fov_factor = 640;

bool is_running = false;

int previous_frame_time = 0;


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

    // Load the cube values in the mesh data

    // load_obj_file_data("./Cube.obj");

    load_cube_mesh_data();

}

void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type){
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
        switch(event.key.keysym.sym){
            case SDLK_ESCAPE: is_running = false; break;
            // Rendering modes
            case SDLK_1: current_render_mode = RENDER_FILLED_WIREFRAME; break;
            case SDLK_2: current_render_mode = RENDER_WIREFRAME; break;
            case SDLK_3: current_render_mode = RENDER_FILLED; break;
            // Back face culling
            case SDLK_c: back_face_culling = !back_face_culling; break;
        }
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function that recieves a 3d vector and returns a projected 2d point
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Perspective projection 
vec2_t project(vec3_t point){
    vec2_t projected_point = {
        .x = (fov_factor * -point.x) / point.z,
        .y = (fov_factor * -point.y) / point.z,
    };
    return projected_point;
}


// Helper function for the qsort in updated for depth sorting
int compare_triangles(const void* a, const void* b) {
    triangle_t* triangle_a = (triangle_t*)a;
    triangle_t* triangle_b = (triangle_t*)b;

    if (triangle_a->avg_depth < triangle_b->avg_depth) return -1;
    if (triangle_a->avg_depth > triangle_b->avg_depth) return 1;
    return 0;
}


void update(void){

    // get the time since the last frame
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);


    // If the time to wait is between 0 and frame target time wait for the time to pass
    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }

    // Initialize array of triangles to render

    triangles_to_render = NULL; 

    // Update the previous frame time
    previous_frame_time = SDL_GetTicks();

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.001;

    mesh.translation.x += 0.01;


    // Create a scale matrix that will be used to scale the mesh vertices
    matrix4_t scale_matrix = matrix4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    matrix4_t rotation_matrix_x = matrix4_make_rotation_x(mesh.rotation.x);
    matrix4_t rotation_matrix_y = matrix4_make_rotation_y(mesh.rotation.y);
    matrix4_t rotation_matrix_z = matrix4_make_rotation_z(mesh.rotation.z);
    matrix4_t translation_matrix = matrix4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);


    // Loop all traingle faces
    int num_faces = array_length(mesh.faces);

    for(int i = 0; i < num_faces; i++){
        // Get the vertices of the triangle
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];
        
        // loop all three vertices of the triangle of this current face and apply transformations
        for(int j = 0; j < 3; j++){
   
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a world Matrix that will be used to transform the vertex
            matrix4_t world_matrix = matrix4_identity();
            // ORDER MATTERS HERE: Scale -> Rotation -> Translation
            world_matrix = matrix4_mul_matrix4(scale_matrix, world_matrix);
            world_matrix = matrix4_mul_matrix4(rotation_matrix_x, world_matrix);
            world_matrix = matrix4_mul_matrix4(rotation_matrix_y, world_matrix);
            world_matrix = matrix4_mul_matrix4(rotation_matrix_z, world_matrix);
            world_matrix = matrix4_mul_matrix4(translation_matrix, world_matrix);

            // Apply the world matrix to the vertex
            transformed_vertex = matrix4_mul_vec4(world_matrix, transformed_vertex);


            // translate the vertex away from the camera
            transformed_vertex.z += -5;

            // Save the transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;

        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // BACK FACE CULLING
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(back_face_culling){
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*    A    */
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  /   \  */
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C-----B */

            // Get the vectors from A to B and A to C and normalize them
            vec3_t vector_ab = vec3_sub(vector_b, vector_a);
            vec3_t vector_ac = vec3_sub(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);

            // Get the triangle normal (using the cross product) and normalize it
            vec3_t normal = vec3_cross(vector_ac, vector_ab);
            vec3_normalize(&normal);

            // Find the vector between a point in the triangle and the camera origin
            vec3_t camera_ray = vec3_sub(camera_position, vector_a);

            // Calculate how alligned the camera ray is with the traingle face normal (dot product)
            float angle = vec3_dot(camera_ray, normal);

            // Bypass the triangles that are looking away from the camera
            if(angle < 0){
                continue;
            }
        }
      
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        vec2_t projected_points[3];

        // loop all three vertices of the triangle and perform the projection
        for (int j = 0; j < 3; j++) {

            // project the current vertex to the screen
            projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));

            // scale and translate the projected point to the center of the screen
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);

        } 

        // Calculate the average depth for each face based on the vertices after transformation
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3;


        triangle_t projected_triangle = {
            .points = { 
                { projected_points[0].x, projected_points[0].y}, 
                { projected_points[1].x, projected_points[1].y},
                { projected_points[2].x, projected_points[2].y}
            },
                .color = mesh.faces[i].color,
                .avg_depth = avg_depth
        };

         // save the projected triangle to the array of triangles to render
        array_push(triangles_to_render, projected_triangle);

        


    }

    // Sort the triangles to render based on the average depth
    int num_triangles = array_length(triangles_to_render);
    if(num_triangles > 0){
        qsort(triangles_to_render, num_triangles, sizeof(triangle_t), compare_triangles);
    }
}


void render(void){

    // Clear the color buffer
    SDL_RenderClear(renderer);
    
    // Loop all projected points and draw a line between them
    int num_triangles = array_length(triangles_to_render);
    for (int i = 0; i < num_triangles; i++){
        triangle_t triangle = triangles_to_render[i];


        if (current_render_mode == RENDER_FILLED || current_render_mode == RENDER_FILLED_WIREFRAME){

            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, 
                triangle.points[1].x, triangle.points[1].y, 
                triangle.points[2].x, triangle.points[2].y, 
                triangle.color
            );
        }

        if (current_render_mode == RENDER_WIREFRAME || current_render_mode == RENDER_FILLED_WIREFRAME){

            draw_triangle(
                triangle.points[0].x, triangle.points[0].y, 
                triangle.points[1].x, triangle.points[1].y, 
                triangle.points[2].x, triangle.points[2].y, 
                0xFF000FF0
            );  

            for (int i = 0; i < 3; i++){
                draw_rectangle(triangle.points[i].x - 3, triangle.points[i].y - 3 ,6 ,6 , 0xFFFF0000);
            }
        } 
    }
    
    array_free(triangles_to_render); // Clear the array of traingles to render every frame loop

    render_color_buffer();
    clear_color_buffer(0xFF000000);
    SDL_RenderPresent(renderer);
}

// Free memory that was allocated
void free_resources(void){
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
    free_resources();

    return 0;
}  