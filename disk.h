#ifndef DISK_H
#define DISK_H

#include <cmath>
#include "vec3.h"
#include "hitable.h"

class disk : public hitable
{
 public:
  disk() {}
 disk(vec3 cen, float w, float h, vec3 n, material *m) : center(cen), width(w), height(h), norm(n), mat_ptr(m) {};
  
  virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

  vec3 center;
  vec3 norm;
  float width;
  float height;
  material *mat_ptr;
};

bool disk::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
  
  return false;
}

#endif
