#include <igloo/surfaces/sphere.hpp>
#include <igloo/geometry.hpp>
#include <distribution2d/distribution2d/unit_sphere_distribution.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

namespace igloo
{


sphere::sphere(const point &c, float r)
  : m_center(c), m_radius(r)
{} // end sphere::sphere()


sphere::sphere(float cx, float cy, float cz, float r)
  : sphere(point(cx,cy,cz), r)
{} // end sphere::sphere()


triangle_mesh sphere::triangulate() const
{
  size_t u_divisions = 100;
  size_t v_divisions = 100;
  float u_del = 1.0f / u_divisions;
  float v_del = 1.0f / v_divisions;

  size_t num_vertices = 4 * u_divisions * v_divisions;

  std::vector<point>                   points(num_vertices);
  std::vector<parametric>              parametrics(num_vertices);
  std::vector<normal>                  normals(num_vertices);
  std::vector<triangle_mesh::triangle> triangles(2 * u_divisions * v_divisions);

  // XXX this method creates lots of redundant data
  // XXX we should try to make this watertight
  dist2d::unit_sphere_distribution<point> unit_sphere;
  point p;
  float v = 0;
  unsigned int vertex_idx = 0;
  for(size_t j = 0; j != u_divisions; ++j, v += v_del)
  {
    float u = 0;
    for(size_t i = 0; i != v_divisions; ++i, u += u_del, vertex_idx += 4)
    {
      point p = unit_sphere(u, v);
      points[vertex_idx + 0]      = p;
      parametrics[vertex_idx + 0] = parametric(u,v);
      normals[vertex_idx + 0]     = normal(p.x, p.y, p.z);

      p = unit_sphere(u + u_del, v);
      points[vertex_idx + 1]      = p;
      parametrics[vertex_idx + 1] = parametric(u + u_del,v);
      normals[vertex_idx + 1]     = normal(p.x, p.y, p.z);

      p = unit_sphere(u + u_del, v + v_del);
      points[vertex_idx + 2]      = p;
      parametrics[vertex_idx + 2] = parametric(u + u_del, v + v_del);
      normals[vertex_idx + 2]     = normal(p.x, p.y, p.z);

      p = unit_sphere(u, v + v_del);
      points[vertex_idx + 3]      = p;
      parametrics[vertex_idx + 3] = parametric(u,v + v_del);
      normals[vertex_idx + 3]     = normal(p.x, p.y, p.z);

      triangles[vertex_idx / 2]     = {vertex_idx + 0, vertex_idx + 1, vertex_idx + 2};
      triangles[vertex_idx / 2 + 1] = {vertex_idx + 0, vertex_idx + 2, vertex_idx + 3};
    } // end for
  } // end for j

  // scale each point by radius and translate each point by center
  float scale = radius();
  float3 translation = center().as_translation();
  std::transform(points.begin(), points.end(), points.begin(), [=](const point &x)
  {
    return (scale * x) + translation;
  });

  return triangle_mesh(std::move(points), std::move(parametrics), std::move(normals), std::move(triangles));
} // end sphere::triangulate()


std::tuple<parametric, vector, vector> sphere::parametric_geometry_at(const normal& n) const
{
  vector p = radius() * n;

  // compute parametric location
  float phi = std::atan2(n.y, n.x);
  if(phi < 0) phi += two_pi;

  float theta = std::acos(n.x);

  parametric uv(phi / max_phi, (theta - min_theta) / (max_theta - min_theta));

  float z_radius = std::sqrt(p[0]*p[0] + p[1]*p[1]);
  float inv_z_radius = 1.f / z_radius;

  float cos_phi = p[0] * inv_z_radius;
  float sin_phi = p[1] * inv_z_radius;

  vector dpdu(-max_phi * p[1], max_phi * p[0], 0);
  vector dpdv = (max_theta - min_theta) * vector(p[2] * cos_phi, p[2] * sin_phi, -radius() * std::sin(theta));

  return std::make_tuple(uv, dpdu, dpdv);
}


optional<intersection> sphere::intersect(const ray &r) const
{
  vector diff = r.origin() - center();

  // compute the coefficients of the quadratic equation of this sphere
  float a = r.direction().norm2();
  float b = 2.0f * dot(r.direction(), diff);
  float c = diff.norm2() - radius() * radius();

  // solve the quadratic
  float root0, root1;
  std::tie(root0,root1) = solve_quadratic(a,b,c);
  if(std::isnan(root0))
  {
    return nullopt;
  }

  // the hits must lie in the interval
  if(root0 > r.interval().y || root1 < r.interval().x)
  {
    return nullopt;
  } // end if

  float t = root0;

  // the hits must lie in the legal bound
  if(t < r.interval().x)
  {
    t = root1;
    if(t > r.interval().y)
    {
      return nullopt;
    } // end if
  } // end if

  // compute the hit point
  point x = r(t);

  // compute the normal at the hit point
  normal n = normalize(x - center());

  parametric uv;
  vector dpdu, dpdv;
  std::tie(uv, dpdu, dpdv) = parametric_geometry_at(n);

  return intersection(t,differential_geometry(x,uv,dpdu,dpdv,n));
} // end sphere::intersect()


std::pair<float,float> sphere::solve_quadratic(float a, float b, float c)
{
  float x0 = std::numeric_limits<float>::quiet_NaN();
  float x1 = x0;

  int result = 0;

  // are there roots?
  float denom = 2.0f*a;

  if(denom != 0.0f)
  {
    float root = b*b - 4.0f*a*c;

    if(root == 0.0f)
    {
      // one root
      x0 = x1 = -b / denom;
    } // end if
    else if(root > 0.0f)
    {
      root = std::sqrt(root);

      x0 = (-b - root) / denom;
      x1 = (-b + root) / denom;

      if(x0 > x1) std::swap(x0,x1);
    } // end else if
  } // end if

  return std::make_pair(x0,x1);
} // end solve_quadratic()


float sphere::area() const
{
  return 4.f * pi * radius() * radius();
} // end area()


differential_geometry sphere::sample_surface(std::uint64_t u0, std::uint64_t u1) const
{
  dist2d::unit_sphere_distribution<normal> unit_sphere;
  normal n = unit_sphere(u0, u1);

  parametric uv;
  vector dpdu, dpdv;
  std::tie(uv, dpdu, dpdv) = parametric_geometry_at(n);
  
  return differential_geometry(center() + radius() * n, uv, dpdu, dpdv, n);
} // end area::point_on_surface()


} // end igloo

