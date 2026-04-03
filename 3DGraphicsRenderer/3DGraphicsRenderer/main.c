#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>
#include "display.h"
#include "array.h"
#include "vector.h"
#include "mesh.h"

// Array of triangles to render
triangle_t* triangles_to_render = NULL;

vec3_t camera_position = { 0, 0, 0 };

     
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

    load_obj_file_data("./Cube.obj");

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function that recieves a 3d vector and returns a projected 2d point
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Perspective projection 
vec2_t project(vec3_t point){
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * -point.y) / point.z,
    };
    
    return projected_point;
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

    // Loop all traingle faces
    int num_faces = array_length(mesh.faces);

    for(int i = 0; i < num_faces; i++){
        // Get the vertices of the triangle
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];
        
        // loop all three vertices of the triangle of this current face and apply transformations
        for(int j = 0; j < 3; j++){
   
            vec3_t transformed_vertex = face_vertices[j];

            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // translate the vertex away from the camera
            transformed_vertex.z += -5;

            // Save the transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;

        }
        //////////////////////////////
        // BACK FACE CULLING
        //////////////////////////////
        vec3_t vector_a = transformed_vertices[0]; /*    A    */
        vec3_t vector_b = transformed_vertices[1]; /*  /   \  */
        vec3_t vector_c = transformed_vertices[2]; /* C-----B */

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
        
        triangle_t projected_triangle;

        // loop all three vertices of the triangle and perform the projection
        for (int j = 0; j < 3; j++) {

            // project the current vertex to the screen
            vec2_t projected_point = project(transformed_vertices[j]);

            // scale and translate the projected point to the center of the screen
            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
            }

        // save the projected triangle to the array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }
}


void render(void){

    // Loop all projected points and draw a line between them

    int num_triangles = array_length(triangles_to_render);

    for (int i = 0; i < num_triangles; i++){

        triangle_t triangle = triangles_to_render[i];

        // draw the vertices

        draw_rectangle(triangle.points[0].x, triangle.points[0].y, 4, 4, 0xFFFF00FF);
        draw_rectangle(triangle.points[1].x, triangle.points[1].y, 4, 4, 0xFFFF00FF);
        draw_rectangle(triangle.points[2].x, triangle.points[2].y, 4, 4, 0xFFFF00FF);

        // draw the wireframe of the triangle
        draw_triangle(
            triangle.points[0].x, triangle.points[0].y, 
            triangle.points[1].x, triangle.points[1].y, 
            triangle.points[2].x, triangle.points[2].y, 
            0xFF0000FF);
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