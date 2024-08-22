#pragma once

#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "../utils.hpp"
#include "vec3.hpp"
#include "BoundingBox.hpp"
#include "Ray.hpp"


class Mesh;

using MeshPtr = std::shared_ptr<Mesh>;
using MeshConstPtr = std::shared_ptr<Mesh const>;


// a triangle is made of 3 points A-B-C:
// the `normal` of triangle and the rotation order A->B->C->A satisfy the right-handed rule;
// the `vertex` is equal to A, for indicating the space position of the triangle;
// the `side1` is B-A and the `side2` is C-A;
// `s11`, `s12`, `s22` are pre-calculate constants.
class Triangle
{
public:

    vec3g vertex, side1, side2, normal;
    fg s11, s12, s22;

    CONST_FUNC Triangle() noexcept
    : vertex{defaults<vec3g>::O}, side1{defaults<vec3g>::X}, side2{defaults<vec3g>::Y}, normal{defaults<vec3g>::Z}
    , s11{1}, s12{0}, s22{1} {}
    CONST_FUNC Triangle(vec3g const& A, vec3g const& B, vec3g const& C) noexcept
    : vertex{A}, side1{B - A}, side2{C - A}, normal{cross(side1, side2).normalize()}
    , s11{length2(side1)}, s12{dot(side1, side2)}, s22(length2(side2)) {
        fg det = s11 * s22 - s12 * s12;
        s11 /= det; s12 /= det; s22 /= det;
    }

    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        fg hit_time = dot(vertex - ray.origin, normal) / dot(ray.direction, normal);
        if ((hit_time < defaults<fg>::eps) || (hit_time >= rec.max_ray_time)) { return false; }

        vec3g hit_point = ray.at(hit_time);
        vec3g coord_point = hit_point - vertex;
        fg     co_u = dot(coord_point, side1),     co_v = dot(coord_point, side2);
        fg contra_u = co_u * s22 - co_v * s12, contra_v = co_v * s11 - co_u * s12;
        if ((contra_u <= 0) || (contra_v <= 0) || (contra_u + contra_v >= 1)) { return false; }

        rec.max_ray_time = hit_time;
        rec.hit_point = hit_point;
        rec.normal = normal;
        rec.hit_local = vec2g(contra_u, contra_v);
        rec.triangle_p = this;
        return true;
    }
};


class Mesh
{
protected:

    class SubBox
    {
    public:

        BoundingBox box;
        u32 index_l, index_r;
    };

    BoundingBox _box;
    std::vector<SubBox> _boxes;
    std::vector<Triangle> _triangles;

    bool _subdivise()
    {
        std::vector<vec3g> centers;
        centers.resize(faces.size());

        for (u64 index = 0; index < faces.size(); index++)
        {
            vec3<u32> const& face = faces[index];
            if (all(face.isless(vertices.size())))
            {
                centers[index] = (vertices[face.x] + vertices[face.y] + vertices[face.z]) / 3;
            }
            else { return false; }
        }

        // TODO: sub divise mesh into boxes


        return true;
    }

public:

    std::vector<vec3<u32>> faces;
    std::vector<vec3g> vertices;

    Mesh() noexcept
    : _box{}, _triangles{}, faces{}, vertices{} {}


    bool prepare()
    {
        _box.reset();
        for (vec3g const& vertex : vertices)
        {
            _box.bound(vertex);
        }

        if (!_subdivise()) { return false; }

        _triangles.resize(faces.size());
        for (u64 index = 0; index < faces.size(); index++)
        {
            vec3<u32> const& face = faces[index];
            _triangles[index] = Triangle(vertices[face.x], vertices[face.y], vertices[face.z]);
        }

        return true;
    }

    /******** trace ray ********/

    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        bool hit = false;
        if (!_box.trace(ray, rec)) { return hit; }

        // TODO: use bitree boxes to optimize it
        for (Triangle const& triangle : _triangles)
        {
            if (triangle.trace(ray, rec))
            {
                hit = true;
                rec.mesh_p = this;
            }
        }
        return hit;
    }


    /******** load obj file ********/

    static MeshPtr load_obj(std::filesystem::path const& path)
    {
        std::ifstream file(path);
        return load_obj(file);
    }
    static MeshPtr load_obj(std::ifstream & file)
    {
        MeshPtr mesh_p = std::make_shared<Mesh>();
        if (!file.is_open()) { return mesh_p; }

        vec3g v; vec3<u32> indeces;

        for (std::string line, part; std::getline(file, line);)
        {
            std::stringstream line_buff(line);
            line_buff >> part;

            if (part == "v")        // geometric vertix
            {
                line_buff >> part; v.x =  s2n<fg>(part);
                line_buff >> part; v.z =  s2n<fg>(part);
                line_buff >> part; v.y = -s2n<fg>(part);
                // should left nothing in `line_buff`
                mesh_p->vertices.push_back(v);
            }
            else if (part == "f")   // polygonal face element
            {
                line_buff >> part; indeces.x = s2n<u32>(part);
                line_buff >> part; indeces.y = s2n<u32>(part);
                line_buff >> part; indeces.z = s2n<u32>(part);
                // TODO: support many indces `.../.../...`
                // TODO: slpit polygon to triangles
                mesh_p->faces.push_back(indeces -= 1);
            }
        }

        return mesh_p;
    }
};
