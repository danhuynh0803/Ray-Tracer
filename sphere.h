#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"
#include "hitable.h"

class sphere : public hitable
{
 public:
  sphere() {}
 sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat_ptr(m) {};
  virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
  vec3 center;
  float radius;
  material *mat_ptr;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(r.direction(), oc);
  float c = dot(oc, oc) - (radius * radius);

  float discriminant = b*b - a*c;
  if (discriminant > 0)
    {      
      float temp = (-b - sqrt(b*b - a*c))/(a);  // closest point on sphere from that ray
      float temp_far = (-b + sqrt(b*b - a*c))/(a); // farthest point on sphere from that ray
      if (temp < t_max && temp > t_min)
	{
	  rec.t = temp;
	  rec.t_far = temp_far;
	  rec.p = r.point_at_parameter(rec.t);
	  rec.normal = (rec.p - center) / radius; // divide by radius to normalize
	  rec.mat_ptr = mat_ptr;
	  return true;
	}
      temp = (-b + sqrt(b*b - a*c))/(a);
      if (temp < t_max && temp > t_min)
	{
	  rec.t = temp;
	  rec.t_far = (-b - sqrt(b*b - a*c))/(a); // for when different perspectives result in negative values
	  rec.p = r.point_at_parameter(rec.t);
	  rec.normal = (rec.p - center) / radius;
	  rec.mat_ptr = mat_ptr;
	  return true;
	}
    }
  return false;
}

#endif
