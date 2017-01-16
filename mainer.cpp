#include <iostream>
#include <fstream>
#include <cfloat>
#include <algorithm>
#include <stdlib.h>

// Defined primitive shapes
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "moving_sphere.h"

#include "hitable_list.h"
#include "camera.h"
#include "material.h"

// Dimensions of image file
const int WIDTH = 640;
const int HEIGHT = 480;
// Number of samples to perform for anti aliasing 
const int SAMPLES = 30;
const int DEPTH = 5;
const vec3 LIGHTPOS(1.0f, 3.0f, 0.0f);                        // Position of our point light
//const vec3 LIGHTPOS(5, 3.5, 3);
const vec3 DIRLIGHT = unit_vector(vec3(1.0f, 1.0f, 1.0f));    // Directional light
//float SPEC_STRENGTH = 0.090f;
float SPEC_STRENGTH = 0.5f;
// Camera position and direction
//const vec3 LOOKFROM(5.0f, 3.5f, 3.0f);
const vec3 LOOKFROM(0.0f, 0.5f, -3.0f);
const vec3 LOOKAT(0.0f, 0.0f, 0.0f);

const int SHADOW_DEPTH = 1;  // Total number of shadows to trace
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
  for (int depth=0; depth < SHADOW_DEPTH; ++depth)
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
  return nonshade - (nonshade - shade)*(float(count)/float(SHADOW_DEPTH));
}

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
	  /* reflective_weight is the percentage by which the object will reflect
	     zero reflective_weight will give only diffuse reflection */	  
	  float weight = rec.mat_ptr->reflect_weight;
	  
	  return (spec*vec3(1.0f, 1.0f, 1.0f)) + shade*attenuation*color(scattered, world, depth+1);
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

hitable *reflect_diffuse_test()
{
  int n = 2;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  //list[0] = new sphere(vec3(0.0f, 0.5f, 0.0f), 0.5, new metal(vec3(1.0f, 0.2f, 0.2f), 0.0f, 0.0f));   // all diffuse
  //list[0] = new sphere(vec3(0.0f, 0.5f, 0.0f), 0.5, new metal(vec3(1.0f, 0.2f, 0.2f), 0.0f, 0.02f));  // 2% reflectance
  //list[0] = new sphere(vec3(0.0f, 0.5f, 0.0f), 0.5, new metal(vec3(1.0f, 0.2f, 0.2f), 0.0f, 0.5f));    // 50% reflectance
  list[0] = new sphere(vec3(0.0f, 0.5f, 0.0f), 0.5, new metal(vec3(1.0f, 0.2f, 0.2f), 0.0f, 1.0f));    // all reflectance  
  list[1] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
  return new hitable_list(list, 2);
}

hitable *fresnel_test()
{
  int n = 4;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, 0.5, 0), 0.5, new dielectric(vec3(0.8f, 0.2f, 0.2f), 1.125f));
  list[1] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
  return new hitable_list(list, 2);
}

hitable *beer_test()
{
  int n = 4;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
  list[1] = new sphere(vec3(1.0, 0.5, 0), 0.5, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.125f, vec3(18.0f, 18.0f, 0.3f)));  // with red, green absorption
  list[2] = new sphere(vec3(0.0, 0.5, 0), 0.5, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.0f, vec3(0.3f, 17.0f, 18.0f)));  // with blue, green absorption
  //list[2] = new sphere(vec3(0.0, 0.5, 0), 0.5, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.0f));    // without absorption

  return new hitable_list(list, 3);
}

hitable *soft_shadow_test()
{
  int n = 2;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
  list[1] = new sphere(vec3(0.0, 0.5, 0), 0.5, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.125f, vec3(18.0f, 18.0f, 0.3f)));  // with red, green absorption
  return new hitable_list(list, 2);
}

hitable *triangle_test()
{
  int n = 2;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));

  // Lambertian
  list[1] = new triangle(
  			 vec3(0.0f, 0.866f, 0.0f), // v0 
  			 vec3(-0.5f, 0.0f, 0.0f),// v1
  			 vec3(0.5f, 0.0f, 0.0f), // v2
  			 new lambertian(new constant_texture(vec3(0.3f, 0.2f, 0.8f))));

  // // Metal
  // list[1] = new triangle(
  // 			 vec3(0.0f, 0.866f, 0.0f), // v0 
  // 			 vec3(-0.5f, 0.0f, 0.0f),// v1
  // 			 vec3(0.5f, 0.0f, 0.0f), // v2
  // 			 new metal(vec3(0.8f, 0.8f, 0.8f), 0.0f));    // all reflectance  

  // // Dielectric (TO FIX)
  // list[1] = new triangle(
  // 			 vec3(0.0f, 0.866f, 0.0f), // v0 
  // 			 vec3(-0.5f, 0.0f, 0.0f),// v1
  // 			 vec3(0.5f, 0.0f, 0.0f), // v2
  // 			 new dielectric(1.0f));

  
  return new hitable_list(list, 2);
}


int main()
{
  int nx = WIDTH;
  int ny = HEIGHT;
  int ns = SAMPLES;
  
  std::ofstream myfile;
  myfile.open("mainer.ppm");  
  myfile << "P3\n" << nx << " " << ny << "\n255\n";

  float R = cos(M_PI/4);
    
  //hitable* world = soft_shadow_test();
  //hitable* world = beer_test();
  hitable* world = triangle_test();
  
  float dist_to_focus = 10.0;
  float aperature = 0.0;
  
  camera cam(LOOKFROM, LOOKAT, vec3(0.0, 1.0, 0.0), 40.0, float(nx)/float(ny), aperature, dist_to_focus, 0.0, 1.0);
  
  for (int j = ny - 1; j >= 0; --j)
    {
      for (int i = 0; i < nx; ++i)
	{
	  vec3 col(0.0f, 0.0f, 0.0f);
	  for(int s = 0; s < ns; ++s)
	    {
	      float u = float(i + drand48()) / float(nx);
	      float v = float(j + drand48()) / float(ny);
	      ray r = cam.get_ray(u, v);
	      vec3 p = r.point_at_parameter(2.0f);
	      col += color(r, world, 0);	  
	    }
	  col /= float(ns);
	  col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
	  int ir = int(255.99 * col.r());
	  int ig = int(255.99 * col.g());
	  int ib = int(255.99 * col.b());

	  // if any component is greater than max value, set it to 255
	  if (ir > 255) ir = 255;
	  if (ig > 255) ig = 255;
	  if (ib > 255) ib = 255;
	  
	  myfile << ir << " " << ig << " " << ib << "\n";
	}
    }

  return 0;
}
