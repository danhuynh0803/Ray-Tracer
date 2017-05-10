#pragma once
#include "sphere.h"
#include "plane.h"
#include "triangle.h"
#include "moving_sphere.h"

#include "hitable_list.h"
#include "material.h"


hitable *cornell_box_test()
{
	int n = 9; 
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(1e5f + 1.0f, 40.8f, 81.6f), 1e5f, new lambertian(new constant_texture(vec3(0.85f, 0.35f, 0.35f))));
	list[1] = new sphere(vec3(-1e5f + 99.0f, 40.8f, 81.6f), 1e5f, new lambertian(new constant_texture(vec3(0.35f, 0.35f, 0.85f))));
	list[2] = new sphere(vec3(50.0f, 40.8f, 1e5f), 1e5f, new lambertian(new constant_texture(vec3(0.75f, 0.75f, 0.75f))));
	list[3] = new sphere(vec3(50.0f, 40.8f, -1e5f + 600.0f), 1e5f, new lambertian(new constant_texture(vec3(1.0f, 1.0f, 1.0f))));
	list[4] = new sphere(vec3(50.0f, 1e5f, 81.6f), 1e5f, new lambertian(new constant_texture(vec3(0.75f, 0.75f, 0.75f))));
	list[5] = new sphere(vec3(50.0f, -1e5f + 81.6f, 81.6f), 1e5f, new lambertian(new constant_texture(vec3(0.75f, 0.75f, 0.75f))));
	list[6] = new sphere(vec3(27.0f, 16.5f, 47.0f), 16.5f, new lambertian(new constant_texture(vec3(0.9f, 0.1f, 0.1f))));
	list[7] = new sphere(vec3(73.0f, 16.5f, 78.0f), 16.5f, new lambertian(new constant_texture(vec3(0.1f, 0.3f, 1.0f))));
	list[8] = new sphere(vec3(50.0f, 681.6f - .77f, 81.6f), 600.0f, new lambertian(new constant_texture(vec3(0.0f, 0.0f, 0.0f))));
}


hitable *reflect_diffuse_test()
{
	int n = 2;
	hitable **list = new hitable*[n + 1];
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
	hitable **list = new hitable*[n + 1];
	texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0, 0.5, 0), 0.5, new dielectric(vec3(0.8f, 0.2f, 0.2f), 1.125f));
	list[1] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
	return new hitable_list(list, 2);
}

hitable *beer_test()
{
	int n = 4;
	hitable **list = new hitable*[n + 1];
	texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
	list[1] = new sphere(vec3(0.0, 0.5, 0.5), 0.5, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.125f, vec3(18.0f, 18.0f, 0.3f)));  // with red, green absorption
																															   //  list[2] = new sphere(vec3(0.0, 0.5, 0), 0.5, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.5f, vec3(0.3f, 5.0f, 9.0f)));  // with blue, green absorption
	list[2] = new sphere(vec3(-2.0, 0.8, 0), 0.8, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.5f, vec3(0.5f, 0.5f, 0.5f)));    // without absorption

	return new hitable_list(list, 3);
}

hitable *soft_shadow_test()
{
	//vec3 LOOKFROM(5.0f, 3.5f, 3.0f);
	//vec3 LOOKAT(0.0f, 0.0f, 0.0f);
	int n = 2;
	hitable **list = new hitable*[n + 1];
	texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
	//list[1] = new sphere(vec3(0.0, 0.5, 0), 0.5, new dielectric(vec3(1.0f, 1.0f, 1.0f), 1.125f, vec3(18.0f, 18.0f, 0.3f)));  // with red, green absorption
	list[1] = new sphere(vec3(0.0, 0.5, 0), 0.5, new lambertian(new constant_texture(vec3(0.9, 0.8, 0.9))));
	return new hitable_list(list, 2);
}

hitable *pyramid_test()
{
	int n = 5;
	hitable **list = new hitable*[n + 1];
	texture *checker = new checker_texture(new constant_texture(vec3(0.3, 0.3, 0.3)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0, -1000, 0), 1000.0f, new lambertian(checker));
	//list[0] = new sphere(vec3(0, -1000, 0), 1000.0f, new metal(vec3(0.5f, 0.5f, 0.5f), 0.0f));
	// Lambertian

	// back
	list[1] = new triangle(
		vec3(0.0f, 0.5, 0.0f), // v0 
		vec3(-0.5f, 0.0f, -0.5f),// v1
		vec3(+0.5f, 0.0f, -0.5f), // v2
		new lambertian(new constant_texture(vec3(0.5f, 0.5f, 1.0f))));
	//new metal(vec3(1.0f, 0.2f, 0.2f), 0.0f));
	// TODO
	// right
	list[2] = new triangle(
		vec3(0.0f, 0.5, 0.0f), // v0 
		vec3(0.5f, 0.0f, -0.5f),// v1
		vec3(0.5f, 0.0f, 0.5f), // v2
		new lambertian(new constant_texture(vec3(0.2f, 1.0f, 0.2f))));

	// front
	list[3] = new triangle(
		vec3(0.0f, 0.5, 0.0f), // v0 
		vec3(0.5f, 0.0f, 0.5f),// v1
		vec3(-0.5f, 0.0f, 0.5f), // v2
		new lambertian(new constant_texture(vec3(0.5f, 1.0f, 1.0f))));

	// left
	list[4] = new triangle(
		vec3(0.0f, 0.5, 0.0f), // v0 
		vec3(-0.5f, 0.0f, -0.5f),// v1
		vec3(-0.5f, 0.0f, +0.5f), // v2
		new lambertian(new constant_texture(vec3(1.0f, 0.5f, 1.0f))));

	return new hitable_list(list, 5);
}
