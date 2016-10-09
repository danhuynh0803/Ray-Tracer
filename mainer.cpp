#include <iostream>
#include <fstream>
#include <cfloat>
#include <stdlib.h>

#include "sphere.h"
#include "plane.h"
#include "moving_sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

vec3 color(const ray& r, hitable *world, int depth)
{
  hit_record rec;
  if (world->hit(r, 0.001f, FLT_MAX, rec))
    {
      ray scattered;
      vec3 attenuation;
      if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
	{
	  return attenuation * color(scattered, world, depth + 1);
	}
      else
	{
	  return vec3(0.0, 0.0, 0.0);
	}
    }
  else
    {
      vec3 unit_direction = unit_vector(r.direction());
      float t = 0.5*(unit_direction.y() + 1.0f); 
      return (1.0f - t)*vec3(0.5f, 0.7f, 1.0f) + t*vec3(0.2f, 0.4f, 1.0f); 
   }
}

hitable *cornell_box()
{
  int n = 10;
  hitable **list = new hitable*[n+1];

  texture *red   = new constant_texture(vec3(.75, .25, .25));
  texture *blue  = new constant_texture(vec3(.25, .25, .75));
  texture *white = new constant_texture(vec3(.75, .75, .75));
  
  list[0] = new sphere(vec3(1e5+1, 40.8, 81.6), 1e5, new lambertian(red));    // right
  list[1] = new sphere(vec3(-1e5+99, 40.8, 81.6), 1e5, new lambertian(blue)); // left
  list[2] = new sphere(vec3(50, 40.8, 1e5), 1e5, new lambertian(white));      // back
  list[3] = new sphere(vec3(50, 1e5, 81.6), 1e5, new lambertian(white));      // bottom
  list[4] = new sphere(vec3(50, -1e5+81, 81.6), 1e5, new lambertian(white));  // top

  //  list[5] = new sphere(vec3(50, 81.6-16.5, 81.6), 1.5, new lambertian(white));  // light

  return new hitable_list(list, 5);
}

hitable *random_scene()
{
  int n = 500;
  hitable **list = new hitable*[n+1];
  texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.9)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.3, 0.3, 0.3))));

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
  list[i++] = new sphere(vec3(0, 3, 0), 0.5, new lambertian(new constant_texture(vec3(1.0, 1.0, 1.0))));
  return new hitable_list(list, i);
}

hitable *two_spheres()
{
  int n = 50;
  hitable **list = new hitable*[n+1];
  texture *red_checker = new checker_texture(new constant_texture(vec3(0.8, 0.3, 0.2)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  texture *blue_checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.8)), new constant_texture(vec3(0.9, 0.9, 0.9)));
  
  list[0] = new sphere(vec3(0, 0.5, 0), 0.5, new lambertian(red_checker));
  list[1] = new sphere(vec3(0, -0.5, 0), 0.5, new lambertian(blue_checker));
  return new hitable_list(list, 2);
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
  int nx = 1920;
  int ny = 1080;
  int ns = 50;
  
  std::ofstream myfile;
  myfile.open("mainer.ppm");  
  myfile << "P3\n" << nx << " " << ny << "\n255\n";

  float R = cos(M_PI/4);

  hitable* world = random_scene();
  
  vec3 lookfrom(13.0f ,3.0, 11.0);
  vec3 lookat(0, 1.0, 0.0);
  float dist_to_focus = 10.0;
  float aperature = 0.0;
  
  camera cam(lookfrom, lookat, vec3(0.0, 1.0, 0.0), 20.0, float(nx)/float(ny), aperature, dist_to_focus, 0.0, 1.0);
  
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

	  myfile << ir << " " << ig << " " << ib << "\n";
	}
    }

  return 0;
}
