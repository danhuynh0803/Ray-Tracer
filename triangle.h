#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cmath>
#include "vec3.h"
#include "hitable.h"

class triangle : public hitable
{
 public:
 triangle(vec3 _v0, vec3 _v1, vec3 _v2, material* m) : v0(_v0), v1(_v1), v2(_v2), mat_ptr(m)
  {
    // Calculate normal, given the 3 vertices
    vec3 v1_v0 = v1 - v0;
    vec3 v2_v0 = v2 - v0;    
    norm = cross(v1_v0, v2_v0);
    //norm = cross(v2_v0, v1_v0); // flip normal
  };

  virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

  vec3 v0;
  vec3 v1;
  vec3 v2;
  vec3 norm;
  material *mat_ptr;
};

bool triangle::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
  // if dot(r.direction(), norm) is approximately equal to zero, then ray direction is parallel to plane and will never intersect the plane of the triangle
  if (dot(r.direction(), norm) > 1e-6)
    {
      // find t at which ray will intersect the plane
      float numerator = dot(v0 - r.origin(), norm);
      float denominator = dot(r.direction(), norm);      
      
      float t = numerator / denominator;

      /* Check if that point is in the triangle:
	 P = a*v0 + b*v1 + c*v2, where a,b,c are weights that sum to 1
	 if any of the weights are negative, then P is not in the triangle */

      vec3 edge0 = v1 - v0;
      vec3 edge1 = v2 - v1;
      vec3 edge2 = v0 - v2;
      vec3 p = r.point_at_parameter(t);    // point on ray of the intersection
      vec3 c0 = p - v0;
      vec3 c1 = p - v1;
      vec3 c2 = p - v2;

      // In triangle if all weights on the three verts are greater than 0
      bool inTriangle =
	(dot(norm, cross(edge0, c0)) > 0) &&
	(dot(norm, cross(edge1, c1)) > 0) &&
	(dot(norm, cross(edge2, c2)) > 0);		
      
      if (t < t_max && t > t_min && inTriangle)
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
