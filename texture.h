#ifndef TEXTURE_H
#define TEXTURE_H

class texture
{
 public:
  virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture
{
 public:
  constant_texture() {}
 constant_texture(vec3 c) : color(c) {}
  virtual vec3 value(float u, float v, const vec3& p) const
  {
    return color;
  }

  vec3 color;  
};

class checker_texture : public texture
{
 public:
  checker_texture() {}
 checker_texture(texture *t0, texture *t1) : even(t0), odd(t1) {}

  virtual vec3 value(float u, float v, const vec3& p) const
  {
    float sines = sin(5*p.x()) * sin(5*p.y()) * sin(5*p.z());
    if (sines < 0)
      return odd->value(u, v, p);
    else
      return even->value(u, v, p);
  }
  
  texture *even;
  texture *odd;
};

class texture_pad : public texture
{
 public:
  texture_pad() {}
 texture_pad(texture *t0, texture *t1) : tex0(t0), tex1(t1) {}

  virtual vec3 value(float u, float v, const vec3& p) const
  {
    

  }
  
  texture *tex0;
  texture *tex1;
};

#endif
