#include <igloo/context.hpp>
#include <igloo/materials/material.hpp>
#include <memory>

using namespace igloo;

// see gluLookAt man page: we construct the
// transpose of their matrix and inverse of their
// translation
void look_at(igloo::context &renderer, float3 eye, float3 center, float3 up)
{
  float3 f = normalize(center - eye);
  up = normalize(up);
  float3 s = cross(f,up);
  float3 u = cross(s,f);

  float4x4 m(s.x, u.x, -f.x, 0,
             s.y, u.y, -f.y, 0,
             s.z, u.z, -f.z, 0,
               0,   0,    0, 1);

  renderer.translate(eye.x, eye.y, eye.z);
  renderer.mult_matrix(m);
} // end look_at()


int main()
{
  igloo::context renderer;

  float unit_square_points[] = {-0.5, 0,  0.5,
                                 0.5, 0,  0.5,
                                 0.5, 0, -0.5,
                                -0.5, 0, -0.5};

  unsigned int unit_square_tris[] = { 0, 1,  3,
                                      1, 2,  3};

  color light_power(10,10,10);

  float light_surface_area = 0.5f * 0.5f; 
  color light_radiosity = light_power / light_surface_area;

  // create some materials
  renderer.material(make_material("igloo::matte",  {{"albedo", red}}), "red");
  renderer.material(make_material("igloo::matte",  {{"albedo", green}}), "green");
  renderer.material(make_material("igloo::matte",  {{"albedo", off_white}}), "white");
  renderer.material(make_material("igloo::mirror", {{"reflectance", white}, {"eta", 2.485f}}), "mirror");
  renderer.material(make_material("igloo::glass",  {{"reflectance", white}, {"transmittance", white}, {"eta", 1.5f}}), "glass");
  renderer.material(make_material("igloo::light",  {{"radiosity", light_radiosity}}), "light");

  // back wall
  renderer.attribute("material", "white");
  renderer.push_matrix();
  renderer.translate(0, 0, -1);
  renderer.rotate(90, 1, 0, 0);
  renderer.scale(2, 2, 2);
  renderer.mesh(unit_square_points, unit_square_tris);
  renderer.pop_matrix();

  // floor
  renderer.attribute("material", "white");
  renderer.push_matrix();
  renderer.translate(0,-1,0);
  renderer.scale(2, 2, 2);
  renderer.mesh(unit_square_points, unit_square_tris);
  renderer.pop_matrix();

  // ceiling
  renderer.attribute("material", "white");
  renderer.push_matrix();
  renderer.translate(0,1,0);
  renderer.scale(2, 2, 2);
  renderer.rotate(180, 1.0, 0, 0);
  renderer.mesh(unit_square_points, unit_square_tris);
  renderer.pop_matrix();

  // left wall
  renderer.attribute("material", "red");
  renderer.push_matrix();
  renderer.translate(-1,0,0);
  renderer.scale(2, 2, 2);
  renderer.rotate(-90, 0, 0, 1);
  renderer.mesh(unit_square_points, unit_square_tris);
  renderer.pop_matrix();

  // right wall
  renderer.attribute("material", "green");
  renderer.push_matrix();
  renderer.translate(1,0,0);
  renderer.scale(2, 2, 2);
  renderer.rotate(90, 0, 0, 1);
  renderer.mesh(unit_square_points, unit_square_tris);
  renderer.pop_matrix();

  // mirror ball
  renderer.attribute("material", "mirror");
  renderer.sphere(-0.4, -0.66, -0.15, 0.33);

  // glass ball
  renderer.attribute("material", "glass");
  renderer.sphere(0.4, -0.66, 0.25, 0.33);

  // left ceiling light
  renderer.attribute("material", "light");
  renderer.push_matrix();
  renderer.translate(-0.5, 0.95, 0);
  renderer.scale(0.5, 0.5, 0.5);
  renderer.rotate(180, 1, 0, 0);
  renderer.mesh(unit_square_points, unit_square_tris);
  renderer.pop_matrix();

  // right ceiling light
  renderer.attribute("material", "light");
  renderer.push_matrix();
  renderer.translate(0.5, 0.95, 0);
  renderer.scale(0.5, 0.5, 0.5);
  renderer.rotate(180, 1, 0, 0);
  renderer.mesh(unit_square_points, unit_square_tris);
  renderer.pop_matrix();

  look_at(renderer, float3(0,0,3), float3(0,0,-1), float3(0,1,0));

  // use path tracing renderer
  renderer.attribute("renderer", "path_tracing");

  renderer.render();

  return 0;
}

