#include <iostream>
#include <fstream>
#include <cfloat>
#include <algorithm>
#include <stdlib.h>
#include <ctime>
#include <thread>
// Defined primitive shapes
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "moving_sphere.h"

#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "all_scenes.h"

# define M_PI  3.14159265358979323846  /* pi */

// === Lighting configurations === //

//#define GLOBAL    // Using Global Illumination
//#define SHADOWS // For direct shadow casting (Automatically off if GI is turned on)
//#define THREADED // Enable for 4 thread rendering

// =============================== //

// Dimensions of image file
const int WIDTH = 640;
const int HEIGHT = 480;

// Store the color onto a seperate 2D canvas and then print it out after all threads have completed
vec3 CANVAS[WIDTH][HEIGHT];

// Number of samples to perform for anti aliasing 
const int SAMPLES = 250;
const int DEPTH = 4;

const vec3 LIGHTPOS(-5, 3.5, 3);
float SPEC_STRENGTH = 0.090f;

// TODO place into seperate scenes
// Camera position and direction
//const vec3 LOOKFROM(-1.0f, 5.0f, -3.0f);
//const vec3 LOOKFROM(2.0f, 2.0f, -1.0f);
//const vec3 LOOKFROM(5.0f, 3.5f, 3.0f);
//const vec3 LOOKAT(0.0f, 0.0f, 0.0f);
vec3 LOOKFROM = vec3(50.0f, 52.0f, 295.6f);
vec3 LOOKAT = unit_vector(vec3(0, -0.042612, -1));

// Render statistics
unsigned long long int numRays = 0;
unsigned long long int numTests = 0;
unsigned long long int numIntersections = 0;

const int SHADOW_DEPTH = 1;  // Total number of shadows to trace. 1 for hard shadows. Around >= 20 seems to give clean enough soft-shadows
bool shadow(const hitable *world, const hit_record& rec)
{
    hit_record temp;
    ray lightDir;
    if (SHADOW_DEPTH > 1)
        lightDir = ray(rec.p, unit_vector((LIGHTPOS - rec.p) + 0.1*random_in_unit_sphere()), 0.0f); // Project ray to light with slight offset to make shadows more soft
    else
        lightDir = ray(rec.p, unit_vector(LIGHTPOS - rec.p), 0.0f); // No offset for hard-shadows
    if (world->hit(lightDir, 0.001f, FLT_MAX, temp))
    {
        return true;
    }
    return false;
}

vec3 softShadow(const hitable *world, const hit_record& rec, float& spec)
{
    vec3 shade(0.3f, 0.3f, 0.3f);
    vec3 nonshade(1.0f, 1.0f, 1.0f);
    int count = 0; // Total number of shadow rays that intersected an object in the scene
    for (int depth = 0; depth < SHADOW_DEPTH; ++depth)
    {
        if (shadow(world, rec))
            count++;
    }
    // Compute specular highlight
    if (count == 0) {
        vec3 viewDir = unit_vector(LOOKFROM - rec.p);
        vec3 lightDir = unit_vector(LIGHTPOS - rec.p);
        vec3 reflectDir = reflect(-lightDir, rec.normal);
        spec = SPEC_STRENGTH * std::pow(std::max(dot(viewDir, reflectDir), 0.0f), 16);
        //  spec *= (1.0f - float(count))/float(SHADOW_DEPTH);
    }
    return nonshade - (nonshade - shade)*(float(count) / float(SHADOW_DEPTH));
}

#ifndef GLOBAL
vec3 color(const ray& r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001f, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        float spec;  // Specular coefficient 

        // check if area should be shadowed
        vec3 shade = softShadow(world, rec, spec);

        if (depth < DEPTH && rec.mat_ptr->scatter(r, rec, attenuation, scattered, LIGHTPOS))
        {
            numIntersections++;

            /* reflective_weight is the percentage by which the object will reflect
               zero reflective_weight will give only diffuse reflection */
            float weight = rec.mat_ptr->reflect_weight;
#ifdef SHADOWS
            return (spec*vec3(1.0f, 1.0f, 1.0f)) + shade*attenuation*color(scattered, world, depth+1); // With shadows
#else 
            return attenuation*color(scattered, world, depth + 1);
#endif
        }
        else
        {
            return vec3(0.0f, 0.0f, 0.0f);
        }
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5f*(unit_direction.y() + 1.0f);
        return (1.0f - t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    }
}
#else 
vec3 color(const ray& r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001f, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        float spec;  // Specular coefficient 

        // check if area should be shadowed
        vec3 shade = softShadow(world, rec, spec);

        if (depth < DEPTH && rec.mat_ptr->scatter(r, rec, attenuation, scattered, LIGHTPOS))
        {
            numIntersections++;

            /* reflective_weight is the percentage by which the object will reflect
               zero reflective_weight will give only diffuse reflection */
            float weight = rec.mat_ptr->reflect_weight;

            return attenuation*color(scattered, world, depth + 1);
            //return (spec*vec3(1.0f, 1.0f, 1.0f)) + shade*attenuation*color(scattered, world, depth+1); // With shadows
        }
        else
        {
            return vec3(0.0f, 0.0f, 0.0f);
        }
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5f*(unit_direction.y() + 1.0f);
        return (1.0f - t)*vec3(1.0f, 1.0f, 1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    }
}
#endif 


/* Progress bar from razzak on stackoverflow 
 * https://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf/14539953
 */
#define PBSTR "======================================================"
#define PBWIDTH 60
void printProgress (double percentage)
{
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}

void putPixel(int x, int y, const vec3& color)
{
    // Make sure the pixel is within the bounds of the canvas
    if ((x > 0 && x < WIDTH) && (y > 0 && y < HEIGHT))
    {
        CANVAS[x][y] = color;        
    }
}

// Trace from the camera to the image plane based on the start and end positions
void trace(int minX, int maxX, int minY, int maxY, hitable* world, camera cam)
{
    for (int j = maxY - 1; j >= minY; --j)
    {
        for (int i = minX; i < maxX; ++i)
        {
            vec3 col(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < SAMPLES; ++s)
            {
                float u = float(i + drand48()) / float(WIDTH);
                float v = float(j + drand48()) / float(HEIGHT);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0f);
                col += color(r, world, 0);
            }
            col /= float(SAMPLES);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col.r());
            int ig = int(255.99 * col.g());
            int ib = int(255.99 * col.b());

            // if any component is greater than max value, set it to 255
            if (ir > 255) ir = 255;
            if (ig > 255) ig = 255;
            if (ib > 255) ib = 255;

            putPixel(i, j, vec3(ir, ig, ib)); 
        }
        // Display the progress percentage
        printProgress((float)(maxY - j) / maxY);
        //std::thread progressBarThread(printProgress, ((float)(HEIGHT - j)/(float)HEIGHT));
    }
    std::cout << std::endl;
}

void printCanvas()
{
    std::string file_name = "output_render.ppm";

    std::ofstream myfile;
    myfile.open(file_name);
    myfile << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";

    vec3 pixel; 
    // Output to a ppm file
    for (int j = HEIGHT - 1; j >= 0; --j)
    {
        for (int i = 0; i < WIDTH; ++i)
        {
            pixel = CANVAS[i][j];
            myfile << pixel.r() << " " << pixel.g() << " " << pixel.b() << "\n";
        }
    }
    myfile.close();
}

int main()
{
    std::string file_name = "output_render.ppm";

    std::ofstream myfile;
    myfile.open(file_name);
    myfile << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";

    float R = cos(M_PI / 4);

    // Scenes are defined in the all_tests.h file
    hitable* world = beer_test();

    // Setup the camera
    float dist_to_focus = 10.0;
    float aperature = 0.0;
    camera cam(LOOKFROM, LOOKAT, vec3(0.0, 1.0, 0.0), 20.0, float(WIDTH) / float(HEIGHT), aperature, dist_to_focus, 0.0, 1.0);

#ifdef THREADED
    std::cout << "Rendering to " << file_name << " at " << WIDTH << " x " << HEIGHT << " resolution: with 4 threads." << std::endl;

    std::vector<std::thread> renderThreads;

    clock_t startTime = clock();
    // Top Left
    renderThreads.push_back(std::thread(trace, 
                0, WIDTH/2, 
                HEIGHT/2, HEIGHT, 
                world, cam));            
    // Top Right
    renderThreads.push_back(std::thread(trace, 
                WIDTH/2, WIDTH, 
                HEIGHT/2, HEIGHT, 
                world, cam));            
    // Bottom Left
    renderThreads.push_back(std::thread(trace, 
                0, WIDTH/2, 
                0, HEIGHT/2, 
                world, cam));            
    // Bottom Right
    renderThreads.push_back(std::thread(trace, 
                WIDTH/2, WIDTH,
                0, HEIGHT/2, 
                world, cam));            

    // Wait for all threads to complete
    for (int i = 0; i < renderThreads.size(); ++i)
    {
        renderThreads[i].join();
    }

    clock_t finishTime = clock();
    // Display performance stats
    float minutes = (finishTime - startTime) / (CLOCKS_PER_SEC * 60);
    float seconds = ( (finishTime - startTime) / CLOCKS_PER_SEC ) % 60;
    std::cout << "Render time    : " << minutes << "m " << ":" << seconds << "s" << std::endl;

    std::cout << "# Primary Rays : " << numRays << std::endl;
    std::cout << "# Inter Tests  : " << numTests << std::endl;
    std::cout << "# Intersections: " << numIntersections << std::endl;

    printCanvas();
#else
    std::cout << "Rendering to " << file_name << " at " << WIDTH << " x " << HEIGHT << " resolution: no multithreading." << std::endl;
    clock_t startTime = clock();
    for (int j = HEIGHT - 1; j >= 0; --j)
    {
        for (int i = 0; i < WIDTH; ++i)
        {
            vec3 col(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < SAMPLES; ++s)
            {
                numRays++;
                float u = float(i + drand48()) / float(WIDTH);
                float v = float(j + drand48()) / float(HEIGHT);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0f);
                col += color(r, world, 0);
            }
            col /= float(SAMPLES);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col.r());
            int ig = int(255.99 * col.g());
            int ib = int(255.99 * col.b());

            // if any component is greater than max value, set it to 255
            if (ir > 255) ir = 255;
            if (ig > 255) ig = 255;
            if (ib > 255) ib = 255;

            myfile << ir << " " << ig << " " << ib << "\n";
        }
        // Display the progress percentage
        printProgress((float)(HEIGHT - j) / HEIGHT);
        //std::thread progressBarThread(printProgress, ((float)(HEIGHT - j)/(float)HEIGHT));
    }
    std::cout << std::endl;

    clock_t finishTime = clock();
    // Display performance stats
    float minutes = (finishTime - startTime) / (CLOCKS_PER_SEC * 60);
    float seconds = ( (finishTime - startTime) / CLOCKS_PER_SEC ) % 60;
    std::cout << "Render time    : " << minutes << "m " << ":" << seconds << "s" << std::endl;

    std::cout << "# Primary Rays : " << numRays << std::endl;
    std::cout << "# Inter Tests  : " << numTests << std::endl;
    std::cout << "# Intersections: " << numIntersections << std::endl;
#endif

    return 0;
}
