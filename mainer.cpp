#include <iostream>
#include <fstream>
#include <cfloat>
#include <algorithm>
#include <stdlib.h>


#include "sphere.h"
#include "plane.h"
#include "moving_sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

// Dimensions of image file
const int WIDTH = 1920;
const int HEIGHT = 1080;
// Number of samples to perform for anti aliasing 
const int SAMPLES = 50;

const vec3 LIGHTPOS(1.0f, 3.0f, 0.0f);                        // Position of our point light
const vec3 DIRLIGHT = unit_vector(vec3(1.0f, 1.0f, 1.0f));    // Directional light
const float SPEC_STRENGTH = 0.2f;

// Camera position and direction
const vec3 LOOKFROM(5, 3.5, 3);
const vec3 LOOKAT(0, 0, 0);

// diff is the diffuse coefficient proportional to the angle between the light direction and the surface normal
bool shadow(const hitable *world, const hit_record& rec, float& diff, float& spec)
{
  // Diffuse component
  ray lightDir(rec.p, unit_vector(LIGHTPOS - rec.p), 0.0f);
  hit_record temp;
  diff = std::max(dot(rec.normal, lightDir.direction()), 0.0f);

  // Specular component
  vec3 viewDir = unit_vector(LOOKFROM - rec.p);
  vec3 reflectDir = reflect(-lightDir.direction(), rec.normal);
  spec = SPEC_STRENGTH * std::pow(std::max(dot(viewDir, reflectDir), 0.0f), 32);  

  if (world->hit(lightDir, 0.001f, FLT_MAX, temp))
    {
      return true;
    }
  return false;
}

vec3 color(const ray& r, hitable *world, int depth)
{
  hit_record rec;
  if (world->hit(r, 0.001f, FLT_MAX, rec))
    {
      ray scattered;
      vec3 attenuation;
      vec3 shade(1.0f, 1.0f, 1.0f);    // The darkening amount of a material if it has shadow, where 0 is completely black and 1 is completely lit

      // Phong lighting model 
      vec3 ambient(0.1f, 0.1f, 0.1f);
      // Have shade vary based on the angle
      float diff;  // Diffuse coefficient (from 0.0 to 1.0);
      float spec;  // Specular coefficient 
      
      // check if area should be shadowed 
      if ( shadow(world, rec, diff, spec) )
	{
	  shade = vec3(0.1f, 0.1f, 0.1f);
	}		 	       
      if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
	{
	  
	  // if (diff <= 1e-6)
	  //   return shade * attenuation * color(scattered, world, depth + 1);
	  // else
	  //   return diff * attenuation * color(scattered, world, depth + 1);
	  
	  return (spec * vec3(1.0f, 1.0f, 1.0f)) + (diff * shade * attenuation * color(scattered, world, depth + 1));
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

hitable *multipass_scene()
{
  int n = 50;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.6, 0.7)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
  list[1] = new sphere(vec3(-4, 1, 2.0), 1.0, new lambertian(new constant_texture(vec3(0.8, 0.8, 0.2))));  
  list[2] = new sphere(vec3(0, 1, 0), 1.0, new metal(vec3(0.7, 0.7, 0.7), 0.0));
  list[3] = new sphere(vec3(4, 0.8, 0), 0.8, new dielectric(1.5));
  list[4] = new sphere(vec3(2, 0.7, 2.0), 0.7, new metal(vec3(0.8, 0.8, 0.8), 0.1));
  list[5] = new sphere(vec3(3, 0.3, 1.5), 0.3, new lambertian(new constant_texture(vec3(0.7, 0.1, 0.7))));
  list[6] = new sphere(vec3(7, 0.5, -0.5), 0.5, new lambertian(new constant_texture(vec3(0.8, 0.5, 0.5))));
  
  return new hitable_list(list, 7);

}

hitable *random_scene()
{
  int n = 500;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.9)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));

  int i = 1;
  for (int a = -11; a < 11; ++a)
    {
      for (int b = -11; b < 11; ++b)
	{
	  float choose_mat = drand48();
	  vec3 center(a+0.9*drand48(), 0.2, b+0.9*drand48());
	  if ((center - vec3(4.0, 0.2, 0.0)).length() > 0.9)
	    {
	      if (choose_mat < 0.8) // diffuse
		{
		  list[i++] = new moving_sphere(center, center+vec3(0, 0.25*drand48(), 0), 0.0, 1.0, 0.2, new lambertian(new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()))));
		}
	      else if (choose_mat < 0.95) // metal
		{ 
		  list[i++] = new sphere(center, 0.2, new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())), 0.5*drand48()));
		}
	      else // glass
		{
		  list[i++] = new sphere(center, 0.2, new dielectric(1.5));
		}
		
	    }
	}
    }
  list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
  list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
  list[i++] = new sphere(vec3( 4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

  return new hitable_list(list, i);
}

hitable *shadow_test()
{
  int n = 3;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.9)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, 0.5, 0), 0.5, new lambertian(new constant_texture(vec3(1.0f, 0.2f, 0.2f))));
  list[1] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
  list[2] = new sphere(vec3(-1, 0, 1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.0f));
  return new hitable_list(list, 3);
}

hitable *plane_scene()
{
  int n = 50;
  hitable **list = new hitable*[n+1];

  texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.9)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  
  list[0] = new plane(vec3(0, -10, 0), 5, 10,  vec3(0, -1, 0), new lambertian(checker));
  list[1] = new sphere(vec3(1, 0, 0), 1, new dielectric(1.1));
  list[2] = new sphere(vec3(2, 0.5, 0), 0.5, new metal(vec3(0.6, 0.5, 0.2), 0.3));
  return new hitable_list(list, 3);
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
    
  hitable* world = shadow_test();
  
  float dist_to_focus = 10.0;
  float aperature = 0.0;
  
  camera cam(LOOKFROM, LOOKAT, vec3(0.0, 1.0, 0.0), 20.0, float(nx)/float(ny), aperature, dist_to_focus, 0.0, 1.0);
  
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

	  // Make color all white if any component is greater than 255 due to specular highlights
	  if (ir > 255 || ig > 255 || ib > 255) 
	    ir = ig = ib = 255;

	  
	  myfile << ir << " " << ig << " " << ib << "\n";
	}
    }

  return 0;
}
