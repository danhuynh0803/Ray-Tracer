#ifndef PLANE_H
#define PLANE_H

#include <cmath>
#include "vec3.h"
#include "hitable.h"

class plane : public hitable
{
 public:
  plane() {}
 plane(vec3 cen, float w, float h, vec3 n, material *m) : center(cen), width(w), height(h), norm(n), mat_ptr(m) {};
  
  virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

  vec3 center;
  vec3 norm;
  float width;
  float height;
  material *mat_ptr;
};

bool plane::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
  if (dot(r.direction(), norm) > 1e-6)
    {
      float numerator = dot(center - r.origin(), norm);
      float denominator = dot(r.direction(), norm);      
      
      float t = numerator / denominator;
      if (t < t_max && t > t_min)
	{ 
	  rec.t = t;
	  rec.p = r.point_at_parameter(rec.t);

	  rec.normal = norm;
	  rec.mat_ptr = mat_ptr;
	  return true;
	}
    }
  
  return false;
}

#endif
