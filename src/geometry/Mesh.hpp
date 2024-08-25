#pragma once

#include <fstream>
#include <filesystem>
#include <math.h>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
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

    CONST_FUNC vec3g center() const noexcept
    {
        return (side1 + side2).mul(defaults<fg>::half).add(vertex);
    }
    CONST_FUNC fg area() const noexcept
    {
        return defaults<fg>::half * length(cross(side1, side2));
    }

    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        // triangle cannot be seen from behind, may be deprecated this line
        if (dot(ray.direction, normal) >= 0) { return false; }

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
        return true;
    }
};


class Mesh
{
public:

    class BoxNode
    {
    public:

        BoundingBox box;
        u32 index_l; // the index to left child node or to triangle if is leaf
        u32 index_r; // the index to right child node or the length of triangles if is leaf, and the highest bit indicate this is leaf or not

        static constexpr u32 leafbit = 0x80000000;

        CONST_FUNC BoxNode() noexcept
        : box{}, index_l{0}, index_r{0} {}
        CONST_FUNC BoxNode(std::vector<vec3g> const& vertices, std::vector<vec3<u32>> const& faces, u32 face_start, u32 n_faces) noexcept
        : box{}, index_l{face_start}, index_r{n_faces | leafbit} {
            vec3<u32> const* face_p = &faces[face_start];
            vec3<u32> const* stop = face_p + n_faces;
            for (; face_p < stop; face_p++)
            {
                box.bound(vertices[face_p->x]);
                box.bound(vertices[face_p->y]);
                box.bound(vertices[face_p->z]);
            }
            box.prepare();
        }

        CONST_FUNC bool isleaf() const noexcept
        {
            return (index_r & leafbit) != 0;
        }

        CONST_FUNC u32 leftchild() const noexcept
        {
            return index_l;
        }
        CONST_FUNC u32 rightchild() const noexcept
        {
            return index_r;
        }
        CONST_FUNC u32 triangle_start() const noexcept
        {
            return index_l;
        }
        CONST_FUNC u32 triangles_length() const noexcept
        {
            return index_r & (~leafbit);
        }
    };


    static constexpr u32 max_triangles_per_box = 3;
    static constexpr u32 max_boxes_depth = 32;
    static constexpr u32 max_strategy_iterations = 16;

    enum class DividingStrategy { X, Y, Z };

    void _subdivide()
    {
        // pre-calculate some useful informations
        std::vector<std::tuple<vec3g, fg>> trian_infos;
        trian_infos.reserve(_triangles.size());
        for (Triangle const& triangle : _triangles)
        {
            trian_infos.emplace_back(triangle.center(), triangle.area());
        }

        _boxes.clear();
        std::vector<u32> boxes_depth;
        // global box
        _boxes.emplace_back(vertices, faces, 0, faces.size());
        boxes_depth.emplace_back(1);

        u32 box_index = 0, box_count = 1;
        for (; box_index < box_count; box_index++)
        {
            BoxNode & box_node = _boxes[box_index];
            u32 box_depth = boxes_depth[box_index];
            // if this box have so much triangles, then divide it in half
            if ((box_node.triangles_length() > max_triangles_per_box) && (box_depth < max_boxes_depth))
            {
                u32 face_start = box_node.triangle_start();
                u32 face_stop = face_start + box_node.triangles_length();
                vec3g box_size = box_node.box.size();

                // find the best divide strategy on each axes so that each halves have almost the same area.

                // divide on x-axis
                DividingStrategy strategy = DividingStrategy::X;
                fg left_area = 0, right_area = 1;
                fg divide_line = box_node.box.min_corner.x;
                for (u32 iter = 0; iter < max_strategy_iterations; iter++)
                {
                    // set a new dividing line
                    fg min_distance = std::numeric_limits<fg>::max(), delta = box_size.x / (1 << (iter + 1));
                    divide_line += delta * (left_area > right_area ? -1 : 1);
                    // calculate how many area in each side
                    left_area = right_area = 0;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        std::tuple<vec3g, fg> const& info = trian_infos[index];
                        fg tmp = std::get<0>(info).x - divide_line;
                        min_distance = std::min(min_distance, std::abs(tmp));
                        if (tmp <= 0) { left_area += std::get<1>(info); }
                        else { right_area += std::get<1>(info); }
                    }
                    if (min_distance >= delta) { break; } // even the nearest triangle is too far to reach
                }

                // divide on y-axis
                fg l_a = 0, r_a = 1;
                fg d_l = box_node.box.min_corner.y;
                for (u32 iter = 0; iter < max_strategy_iterations; iter++)
                {
                    // set a new dividing line
                    fg min_distance = std::numeric_limits<fg>::max(), delta = box_size.y / (1 << (iter + 1));
                    d_l += delta * (l_a > r_a ? -1 : 1);
                    // calculate how many area in each side
                    l_a = r_a = 0;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        std::tuple<vec3g, fg> const& info = trian_infos[index];
                        fg tmp = std::get<0>(info).y - d_l;
                        min_distance = std::min(min_distance, std::abs(tmp));
                        if (tmp <= 0) { l_a += std::get<1>(info); }
                        else { r_a += std::get<1>(info); }
                    }
                    if (min_distance >= delta) { break; } // even the nearest triangle is too far to reach
                }
                // replace the strategy if the divide on y-axis is better than on x-aixs
                if (std::abs(l_a - r_a) < std::abs(left_area - right_area))
                {
                    strategy = DividingStrategy::Y;
                    divide_line = d_l;
                    left_area = l_a; right_area = r_a;
                }

                // divide on z-axis
                l_a = 0; r_a = 1;
                d_l = box_node.box.min_corner.z;
                for (u32 iter = 0; iter < max_strategy_iterations; iter++)
                {
                    // set a new dividing line
                    fg min_distance = std::numeric_limits<fg>::max(), delta = box_size.z / (1 << (iter + 1));
                    d_l += delta * (l_a > r_a ? -1 : 1);
                    // calculate how many area in each side
                    l_a = r_a = 0;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        std::tuple<vec3g, fg> const& info = trian_infos[index];
                        fg tmp = std::get<0>(info).z - d_l;
                        min_distance = std::min(min_distance, std::abs(tmp));
                        if (tmp <= 0) { l_a += std::get<1>(info); }
                        else { r_a += std::get<1>(info); }
                    }
                    if (min_distance >= delta) { break; } // even the nearest triangle is too far to reach
                }
                // replace the strategy if the divide on z-axis is better than above
                if (std::abs(l_a - r_a) < std::abs(left_area - right_area))
                {
                    strategy = DividingStrategy::Z;
                    divide_line = d_l;
                    left_area = l_a; right_area = r_a;
                }

                // divide this box in halves if the divide strategy success
                if ((left_area > defaults<fg>::eps) && (right_area > defaults<fg>::eps))
                {
                    auto strategy_axis = [strategy] (vec3g const& center) noexcept -> fg
                    {
                        switch (strategy)
                        {
                            case DividingStrategy::X:
                                return center.x;
                            case DividingStrategy::Y:
                                return center.y;
                            case DividingStrategy::Z:
                                return center.z;
                        }
                    };

                    constexpr u32 invalid_index = std::numeric_limits<u32>::max();
                    // swap triangles to left part and right part
                    u32 index_right_start = invalid_index;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        if (strategy_axis(std::get<0>(trian_infos[index])) <= divide_line)
                        {
                            if (index_right_start != invalid_index)
                            {
                                std::swap(      faces[index_right_start],       faces[index]);
                                std::swap( _triangles[index_right_start],  _triangles[index]);
                                std::swap(trian_infos[index_right_start], trian_infos[index]);
                                index_right_start++;
                            }
                        }
                        else if (index_right_start == invalid_index)
                        {
                            index_right_start = index;
                        }
                    }

                    // contruct child-boxes
                    box_node.index_l = box_count++;
                    box_node.index_r = box_count++;
                    boxes_depth.emplace_back(box_depth + 1);
                    boxes_depth.emplace_back(box_depth + 1);
                    _boxes.emplace_back(vertices, faces, face_start, index_right_start - face_start);
                    _boxes.emplace_back(vertices, faces, index_right_start, face_stop - index_right_start);
                }
            }
        }
    }

    std::vector<Triangle> _triangles;
    std::vector<BoxNode> _boxes;

public:

    std::vector<vec3<u32>> faces;
    std::vector<vec3g> vertices;
    bool prepared;

    Mesh() noexcept
    : _triangles{}, _boxes{}, faces{}, vertices{}, prepared{false} {}


    bool prepare()
    {
        if (prepared) { return true; }

        _triangles.resize(faces.size());
        for (u64 index = 0; index < faces.size(); index++)
        {
            vec3<u32> const& face = faces[index];
            if (all(face.isless(vertices.size())))
            {
                _triangles[index] = Triangle(vertices[face.x], vertices[face.y], vertices[face.z]);
            }
            else { return false; }
        }

        _subdivide();

#ifdef SHOW_TRACE_INFO
        std::cout << "# of triangles: " << _triangles.size() << ", # of boxes: " << _boxes.size() << std::endl;
#endif

        return prepared = true;
    }


    /******** mesh trasformations ********/

    Mesh & project_to_sphere(fg radius = 1) noexcept
    {
        prepared = false;
        for (vec3g & vertex : vertices)
        {
            vertex *= radius / length(vertex);
        }
        return *this;
    }

    Mesh & subdivise(u32 subdivision = 2)
    {
        if (subdivision < 2) { return *this; }
        prepared = false;

        // TODO: share the new vertices belong the edges
        decltype(vertices) new_vertices;
        decltype(faces) new_faces;
        new_vertices.reserve(faces.size() * (((subdivision + 1) * (subdivision + 2)) >> 1));
        new_faces.reserve(faces.size() * sqr(subdivision));

        u32 vertex_index = 0;
        for (vec3<u32> const& face : faces)
        {
            vec3g const& A = vertices[face.x];
            vec3g const& B = vertices[face.y];
            vec3g const& C = vertices[face.z];
            vec3g ACn = (C - A) / subdivision;
            vec3g CBn = (B - C) / subdivision;

            vec3g m1 = A;
            for (u32 u = 0; u <= subdivision; u++)
            {
                vec3g m2 = m1;
                for (u32 v = 0; v <= u; v++)
                {
                    new_vertices.push_back(m2);
                    m2 += CBn;
                }
                m1 += ACn;
            }

            for (u32 u = 0; u < subdivision; u++)
            {
                for (u32 v = 0; v < u; v++)
                {
                    new_faces.emplace_back(vertex_index, vertex_index + u + 2, vertex_index + u + 1);
                    new_faces.emplace_back(vertex_index, vertex_index + 1, vertex_index + u + 2);
                    vertex_index++;
                }
                new_faces.emplace_back(vertex_index, vertex_index + u + 2, vertex_index + u + 1);
                vertex_index++;
            }
            vertex_index += subdivision + 1;
        }

        vertices.swap(new_vertices);
        faces.swap(new_faces);
        return *this;
    }


    /******** trace ray ********/

    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        bool hit = false;
        auto [time_in, time_out] = _boxes.front().box.trace(ray);
#ifdef SHOW_TRACE_INFO
        rec.box_count++;
#endif
        if ((time_out < defaults<fg>::eps) || (time_in >= time_out)) { return hit; }

        using StackEltype = std::tuple<BoxNode const* /* box */, fg /* time_in */>;
        StackEltype to_trace_boxes[max_boxes_depth + 1];
        StackEltype * box_p = to_trace_boxes;
        *box_p = {_boxes.data(), time_in};

        while (box_p >= to_trace_boxes)
        {
            BoxNode const& box_node = *std::get<0>(*box_p);
            time_in = std::get<1>(*(box_p--));

            if (time_in >= rec.max_ray_time) { continue; }

            if (box_node.isleaf())
            {
                Triangle const* triangle_p = &_triangles[box_node.triangle_start()];
                Triangle const* stop = triangle_p + box_node.triangles_length();
                for (; triangle_p < stop; triangle_p++)
                {
                    hit |= triangle_p->trace(ray, rec);
#ifdef SHOW_TRACE_INFO
                    rec.triangle_count++;
#endif
                }
            }
            else
            {
                // trace two chidren box
                BoxNode const* child_l = &_boxes[box_node.leftchild()];
                BoxNode const* child_r = &_boxes[box_node.rightchild()];
                auto [in_l, out_l] = child_l->box.trace(ray);
                auto [in_r, out_r] = child_r->box.trace(ray);
#ifdef SHOW_TRACE_INFO
                rec.box_count += 2;
#endif

                // sort them so that the first hit is the left one
                if (in_r < in_l)
                {
                    std::swap(child_l, child_r);
                    std::swap(in_l, in_r);
                    std::swap(out_l, out_r);
                }

                // push them in to stack (or not)
                if ((out_r >= defaults<fg>::eps) && (in_r < out_r))
                {
                    *(++box_p) = {child_r, in_r};
                }
                if ((out_l >= defaults<fg>::eps) && (in_l < out_l))
                {
                    *(++box_p) = {child_l, in_l};
                }
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
    static MeshPtr load_obj(std::ifstream & file);


    /******** mesh generations ********/

    static MeshPtr tetrahedron();
    static MeshPtr hexahedron();
    static MeshPtr octahedron();
    static MeshPtr dodecahedron();
    static MeshPtr icosahedron();

    static MeshPtr uv_sphere(u32 n_longitude, u32 n_latitude);
};


MeshPtr Mesh::load_obj(std::ifstream & file)
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

MeshPtr Mesh::tetrahedron()
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->vertices.reserve(4);
    mesh_p->faces.reserve(4);

    f32 root2 = std::sqrt(2.0f);
    f32 z = -defaults<f32>::third;
    f32 x1 = 2 * root2 / 3;
    f32 x2 = -root2 / 3;
    f32 y = std::sqrt(2.0f / 3.0f);

    mesh_p->vertices.emplace_back(0,   0, 1);
    mesh_p->vertices.emplace_back(x1,  0, z);
    mesh_p->vertices.emplace_back(x2, -y, z);
    mesh_p->vertices.emplace_back(x2,  y, z);

    mesh_p->faces.emplace_back(1, 2, 3);
    mesh_p->faces.emplace_back(1, 0, 2);
    mesh_p->faces.emplace_back(1, 3, 0);
    mesh_p->faces.emplace_back(0, 3, 2);

    return mesh_p;
}

MeshPtr Mesh::hexahedron()
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->vertices.reserve(8);
    mesh_p->faces.reserve(12);

    f32 v = 1.0f / std::sqrt(3.0f);

    mesh_p->vertices.emplace_back(-v, -v, -v);
    mesh_p->vertices.emplace_back( v, -v, -v);
    mesh_p->vertices.emplace_back( v,  v, -v);
    mesh_p->vertices.emplace_back(-v,  v, -v);
    mesh_p->vertices.emplace_back(-v, -v,  v);
    mesh_p->vertices.emplace_back( v, -v,  v);
    mesh_p->vertices.emplace_back( v,  v,  v);
    mesh_p->vertices.emplace_back(-v,  v,  v);

    mesh_p->faces.emplace_back(0, 3, 2);
    mesh_p->faces.emplace_back(0, 2, 1);
    mesh_p->faces.emplace_back(0, 1, 5);
    mesh_p->faces.emplace_back(0, 5, 4);
    mesh_p->faces.emplace_back(1, 2, 6);
    mesh_p->faces.emplace_back(1, 6, 5);
    mesh_p->faces.emplace_back(2, 3, 7);
    mesh_p->faces.emplace_back(2, 7, 6);
    mesh_p->faces.emplace_back(3, 0, 4);
    mesh_p->faces.emplace_back(3, 4, 7);
    mesh_p->faces.emplace_back(4, 5, 7);
    mesh_p->faces.emplace_back(5, 6, 7);

    return mesh_p;
}

MeshPtr Mesh::octahedron()
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->vertices.reserve(6);
    mesh_p->faces.reserve(8);

    mesh_p->vertices.emplace_back( 0,  0,  1);
    mesh_p->vertices.emplace_back( 1,  0,  0);
    mesh_p->vertices.emplace_back( 0,  1,  0);
    mesh_p->vertices.emplace_back(-1,  0,  0);
    mesh_p->vertices.emplace_back( 0, -1,  0);
    mesh_p->vertices.emplace_back( 0,  0, -1);

    mesh_p->faces.emplace_back(0, 1, 2);
    mesh_p->faces.emplace_back(0, 2, 3);
    mesh_p->faces.emplace_back(0, 3, 4);
    mesh_p->faces.emplace_back(0, 4, 1);
    mesh_p->faces.emplace_back(5, 2, 1);
    mesh_p->faces.emplace_back(5, 3, 2);
    mesh_p->faces.emplace_back(5, 4, 3);
    mesh_p->faces.emplace_back(5, 1, 4);

    return mesh_p;
}

// TODO: MeshPtr Mesh::dodecahedron()

// TODO: MeshPtr Mesh::icosahedron()

MeshPtr Mesh::uv_sphere(u32 n_longitude, u32 n_latitude)
{
    n_longitude = std::max(2u, n_longitude);
    n_latitude  = std::max(1u, n_latitude);

    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->vertices.reserve(n_longitude * n_latitude + 2);
    mesh_p->faces.reserve(n_longitude * n_latitude * 2);

    mesh_p->vertices.emplace_back(0, 0,  1);    // top vertex
    mesh_p->vertices.emplace_back(0, 0, -1);    // bottom vertex
    for (u32 latitude_count = 0; latitude_count < n_latitude; latitude_count++)
    {
        fg theta = defaults<fg>::pi * (latitude_count + 1) / (n_latitude + 1);
        fg stheta = std::sin(theta), ctheta = std::cos(theta);
        for (u32 longitude_count = 0; longitude_count < n_longitude; longitude_count++)
        {
            fg phi = defaults<fg>::two_pi * longitude_count / n_longitude;
            fg sphi = std::sin(phi), cphi = std::cos(phi);
            mesh_p->vertices.emplace_back(cphi * stheta, sphi * stheta, ctheta);
        }
    }

    u32 last_round_vertices_index = (n_latitude - 1) * n_longitude + 2;
    for (u32 longitude_count = 1; longitude_count < n_longitude; longitude_count++)
    {
        mesh_p->faces.emplace_back(0, longitude_count + 1, longitude_count + 2);    // top faces
        mesh_p->faces.emplace_back(1, last_round_vertices_index + longitude_count, last_round_vertices_index + longitude_count - 1);    // bottom faces
    }
    mesh_p->faces.emplace_back(0, n_longitude + 1, 2);  // top face when longitude_count = 0
    mesh_p->faces.emplace_back(1, last_round_vertices_index, last_round_vertices_index + n_longitude - 1);  // bottom face when longitude_count = 0
    for (u32 latitude_count = 1; latitude_count < n_latitude; latitude_count++)
    {
        u32 vertices_index = n_longitude * latitude_count + 2;
        for (u32 longitude_count = 1; longitude_count < n_longitude; longitude_count++)
        {
            // rectangle
            mesh_p->faces.emplace_back(vertices_index + longitude_count - 1, vertices_index + longitude_count, vertices_index - n_longitude + longitude_count - 1);
            mesh_p->faces.emplace_back(vertices_index + longitude_count, vertices_index - n_longitude + longitude_count, vertices_index - n_longitude + longitude_count - 1);
        }
        // rectangle when longitude_count = 0
        mesh_p->faces.emplace_back(vertices_index + n_longitude - 1, vertices_index, vertices_index - 1);
        mesh_p->faces.emplace_back(vertices_index, vertices_index - n_longitude, vertices_index - 1);
    }

    return mesh_p;
}
