#include "Triangle.h"

Triangle::Triangle(vector<Vec3f> v)
{
    float d1,d2,d3;
    vertices.push_back(v[0]);
    vertices.push_back(v[1]);
    vertices.push_back(v[2]);

    // Calculate normal
    normal.resize(3);
    Vec3f n = v[0].getSurfNorm(v[1], v[2]);
    n.normalize();
    normal[0] = n;
    normal[1] = n;
    normal[2] = n;

    // uv's
    uv.resize(3);
    uv[0] = Vec3f();
    uv[1] = Vec3f();
    uv[2] = Vec3f();

    // texture ID
    texID = 0;

    // Calculate center and radius
    center = (vertices[0] + vertices[1] + vertices[2]) / 3.0;
    d1 = (vertices[0] - center).length();
    d2 = (vertices[1] - center).length();
    d3 = (vertices[2] - center).length();
    radius = d3 > d2 ? d3 > d1 ? d3 : d1 : d3 > d1 ? d3 : d2 > d1 ? d2 : d1;

}

bool Triangle::intersect(const Ray &ray, Hit &hit) const
{
    Vec3f e1 = vertices[1] - vertices[0];
    Vec3f e2 = vertices[2] - vertices[0];

    Vec3f h = ray.d.cross(e2);
    float a = e1.dot(h);
    
    if (a > -0.00001 && a < 0.00001)
        return false;
  
    float f = 1.0f/a;
    Vec3f s = ray.s - vertices[0];
    float u = f * s.dot(h);
  
    if (u < 0.0 || u > 1.0)
        return false;
  
    Vec3f q = s.cross(e1);
    float v = f * ray.d.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return false;
  
    float t = f * e2.dot(q);
    if ( t > 0.00001)
    {
        hit.triangle = this;
        hit.nearIntersect = t;
        return true;
    }
    else
    {
        return false;
    }
}
