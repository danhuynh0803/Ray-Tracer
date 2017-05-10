#ifndef MATERIAL_H
#define MATERIAL_H

struct hit_record;

#include <algorithm>
#include <math.h>
#include "ray.h"
#include "hitable.h"
#include "texture.h"

vec3 reflect(const vec3& v, const vec3& n);
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted);
vec3 random_in_unit_sphere(void);
float schlick(float cosine, float ref_idx);


class material
{
 public:
  virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, const vec3& light_pos) const = 0;

  float reflect_weight;  // used to generate a mix of specular and diffuse 
};

class constant_color : public material
{
 public:
  constant_color() { reflect_weight = 0.0f; }
 constant_color(const vec3& col) : albedo(col) { reflect_weight = 0.0f; }
  virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, const vec3& light_pos) const
  {
    float diff = std::max(dot(rec.normal, unit_vector(light_pos - rec.p)), 0.0f); // Diffuse component
	float d = (light_pos - rec.p).length();
	//float atten = 1.0f / (1.0 + 0.09*d + 0.032*d*d);
	//float atten = 1.0f;
    attenuation = diff * albedo;    
    return true;
  }
  
  vec3 albedo;
  float reflect_weight;
};

/* Original 
class lambertian: public material
{
 public:
 lambertian(texture *a) : albedo(a) { reflect_weight = 0.0f; }
  virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, const vec3& light_pos) const
  {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();    
    scattered = ray(rec.p, target - rec.p, r_in.time());
    float diff = std::max(dot(rec.normal, unit_vector(light_pos - rec.p)), 0.0f); // Diffuse component
    attenuation = diff * albedo->value(0, 0, rec.p);
    return true;
  }
  
  texture *albedo;
  float reflect_weight;
};
*/

// Add slight gloss
class lambertian : public material
{
public:
	lambertian(texture *a) : albedo(a) { reflect_weight = 0.0f; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, const vec3& light_pos) const
	{
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + 0.01*random_in_unit_sphere(), r_in.time());

		float diff = std::max(dot(rec.normal, unit_vector(light_pos - rec.p)), 0.0f); // Diffuse component
		attenuation = diff * albedo->value(0, 0, rec.p);
		return true;
	}

	texture *albedo;
	float reflect_weight;
};

class metal : public material
{
 public:
 metal(const vec3& a) : albedo(a)
  {
    fuzz = 0.0f;
    reflect_weight = 1.0f;
  }
  
 metal(const vec3& a, float f) : albedo(a)
  {
    if (f < 1)
      fuzz = f;
    else
      fuzz = f;

    reflect_weight = 1.0f;
  }
  
 metal(const vec3& a, float f, float r) : albedo(a)
  {
    if (f < 1)
      fuzz = f;
    else
      fuzz = f;

    if (r < 0)
      reflect_weight = 0.0f;
    else if (r > 1)
      reflect_weight = 1.0f;
    else
      reflect_weight = r;
  }

  virtual bool scatter(const ray& ray_in, const hit_record& rec, vec3& attenuation, ray& scattered, const vec3& light_pos) const
  {
    vec3 reflected = reflect(unit_vector(ray_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), ray_in.time());
    float diff = std::max(dot(rec.normal, unit_vector(light_pos - rec.p)), 0.0f); // Diffuse component
    attenuation = diff * albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }
  
  vec3 albedo;
  float fuzz;
  float reflect_weight;
};

class dielectric : public material
{
 public:
 dielectric(float ri) : ref_idx(ri)
  {
    albedo = vec3(1.0f, 1.0f, 1.0f);    
    reflect_weight = 0.0f;
    absorption = vec3(0.0f, 0.0f, 0.0f);
  }
 dielectric(const vec3& a, float ri) : albedo(a), ref_idx(ri)
  {
    reflect_weight = 0.0f;
    absorption = vec3(0.0f, 0.0f, 0.0f);
  }  
 dielectric(const vec3& a, float ri, const vec3& absorp) : albedo(a), ref_idx(ri), absorption(absorp) { reflect_weight = 0.0f; }
    
  virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, const vec3& light_pos) const
  {
    vec3 outward_normal;
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    float ni_over_nt;

    // Beer's law
    float absorb_distance = rec.t_far - rec.t;    
    float ar = exp(-absorption[0] * absorb_distance);
    float ag = exp(-absorption[1] * absorb_distance);
    float ab = exp(-absorption[2] * absorb_distance);
    vec3 absorb(ar, ag, ab);
    attenuation = absorb * albedo;

    // If absorb_distance is negative, then we've reached the backside of the sphere, so we do not apply beer's law
    if (absorb_distance < 0) { attenuation = albedo; }
    
    vec3 refracted;
    // reflect based on fresnel effect
    float reflect_prob;
    
    float cosine;
    
    if (dot(r_in.direction(), rec.normal) > 0)
	{
		outward_normal = -rec.normal;
		ni_over_nt = ref_idx;
		cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
	}
    else
	{
		outward_normal = rec.normal;
		ni_over_nt = 1.0 / ref_idx;
		cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
	}
    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
	{
		reflect_prob = schlick(cosine, ref_idx);
	}
    else
	{
		scattered = ray(rec.p, reflected, r_in.time());
		reflect_prob = 1.0;
	}
    if (drand48() < reflect_prob)
	{
		scattered = ray(rec.p, reflected, r_in.time());
	}
	else
	{
		scattered = ray(rec.p, refracted, r_in.time());
	}
	return true;
  }
  
  float ref_idx;
  vec3 albedo;
  float reflect_weight;
  vec3 absorption;
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
  vec3 i = unit_vector(v);
  float dt = dot(i, n); // angle btw i and n
  float discriminant = 1.0 - ( (ni_over_nt)*(ni_over_nt) * (1 - dt*dt) );
  if (discriminant > 0)
    {      
      refracted = (ni_over_nt*i) - ( (ni_over_nt*dt) + sqrt(discriminant) )*n;
      return true;
    }
  else
    {
      return false;
    }  
}


float schlick(float cosine, float ref_idx)
{
  float r0 = (1 - ref_idx)/(1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5); 
}
	      
vec3 reflect(const vec3& v, const vec3& n)
{
  return v - 2*dot(v,n)*n;
}


vec3 random_in_unit_sphere()
{
  vec3 p;
  do {
    p = 2.0*vec3(drand48(), drand48(), drand48()) - vec3(1.0f, 1.0f, 1.0f);    
  } while(p.squared_length() >= 1.0);
  return p;
}

#endif
