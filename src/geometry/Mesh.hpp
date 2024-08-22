#pragma once

#include <fstream>
#include <filesystem>
#include <math.h>
#include <memory>
#include <queue>
#include <stack>
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
    // max_boxes_depth ?
    static constexpr u32 max_strategy_iterations = 32;

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

        // Boxes Queue
        std::queue<BoxNode> unprocessed_boxes;
        // global box
        unprocessed_boxes.emplace(vertices, faces, 0, faces.size());

        u32 processed_count = 0;
        while (!unprocessed_boxes.empty())
        {
            BoxNode & box_node = unprocessed_boxes.front();

            // if this box have so much triangles, then divide it in half
            static_assert(max_triangles_per_box > 0);
            if (u32 n_faces = box_node.triangles_length(); n_faces > max_triangles_per_box)
            {
                u32 face_stop = box_node.triangle_start() + n_faces;
                vec3g box_size = box_node.box.size();

                // find the divide strategy on each axes so that each halves have almost the same area.

                fg xla = 0, xra = 1;
                fg line_x = box_node.box.min_corner.x;
                for (u32 iter = 0; iter < max_strategy_iterations; iter++)
                {
                    // set a new dividing line
                    fg min_distance = std::numeric_limits<fg>::max(), delta = box_size.x / (1 << (iter + 1));
                    line_x += delta * (xla > xra ? -1 : 1);
                    // calculate how many area in each side
                    xla = xra = 0;
                    for (u32 index = box_node.triangle_start(); index < face_stop; index++)
                    {
                        std::tuple<vec3g, fg> const& info = trian_infos[index];
                        fg tmp = std::get<0>(info).x - line_x;
                        min_distance = std::min(min_distance, std::abs(tmp));
                        if (tmp <= 0) { xla += std::get<1>(info); }
                        else { xra += std::get<1>(info); }
                    }
                    if (min_distance > delta) { break; }
                }

                fg yla = 0, yra = 1;
                fg line_y = box_node.box.min_corner.y;
                for (u32 iter = 0; iter < max_strategy_iterations; iter++)
                {
                    // set a new dividing line
                    fg min_distance = std::numeric_limits<fg>::max(), delta = box_size.y / (1 << (iter + 1));
                    line_y += delta * (yla > yra ? -1 : 1);
                    // calculate how many area in each side
                    yla = yra = 0;
                    for (u32 index = box_node.triangle_start(); index < face_stop; index++)
                    {
                        std::tuple<vec3g, fg> const& info = trian_infos[index];
                        fg tmp = std::get<0>(info).y - line_y;
                        min_distance = std::min(min_distance, std::abs(tmp));
                        if (tmp <= 0) { yla += std::get<1>(info); }
                        else { yra += std::get<1>(info); }
                    }
                    if (min_distance > delta) { break; }
                }

                fg zla = 0, zra = 1;
                fg line_z = box_node.box.min_corner.z;
                for (u32 iter = 0; iter < max_strategy_iterations; iter++)
                {
                    // set a new dividing line
                    fg min_distance = std::numeric_limits<fg>::max(), delta = box_size.z / (1 << (iter + 1));
                    line_z += delta * (zla > zra ? -1 : 1);
                    // calculate how many area in each side
                    zla = zra = 0;
                    for (u32 index = box_node.triangle_start(); index < face_stop; index++)
                    {
                        std::tuple<vec3g, fg> const& info = trian_infos[index];
                        fg tmp = std::get<0>(info).z - line_z;
                        min_distance = std::min(min_distance, std::abs(tmp));
                        if (tmp <= 0) { zla += std::get<1>(info); }
                        else { zra += std::get<1>(info); }
                    }
                    if (min_distance > delta) { break; }
                }

                // compare each strategies and choose the best one

                DividingStrategy strategy = DividingStrategy::X;
                fg divide_line = line_x;
                fg left_area = xla, right_area = xra;
                if (std::abs(yla - yra) < std::abs(left_area - right_area))
                {
                    strategy = DividingStrategy::Y;
                    divide_line = line_y;
                    left_area = yla; right_area = yra;
                }
                if (std::abs(zla - zra) < std::abs(left_area - right_area))
                {
                    strategy = DividingStrategy::Z;
                    divide_line = line_z;
                    left_area = zla; right_area = zra;
                }

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

                // swap triangles to left part and right part

                u32 face_start = box_node.triangle_start();
                u32 index_right_start = -1;
                for (u32 index = face_start + 1; index < face_stop; index++)
                {
                    if ((strategy_axis(std::get<0>(trian_infos[index])) <= divide_line) && (index_right_start != -1))
                    {
                        std::swap(      faces[index_right_start],       faces[index]);
                        std::swap( _triangles[index_right_start],  _triangles[index]);
                        std::swap(trian_infos[index_right_start], trian_infos[index]);
                        index_right_start++;
                    }
                    else if (index_right_start == -1)
                    {
                        index_right_start = index;
                    }
                }

                if ((index_right_start == face_start) || (index_right_start == face_stop) || (index_right_start == -1))
                {
                    std::cout << "count " << processed_count << " (" << face_start << '|' << index_right_start
                        << '|' << face_stop << ')' << std::endl;
                    std::cout << "box: [" << box_node.box.min_corner.x << ' ' << box_node.box.min_corner.y << ' ' << box_node.box.min_corner.z
                              << "] - ["  << box_node.box.max_corner.x << ' ' << box_node.box.max_corner.y << ' ' << box_node.box.max_corner.z << ']' << std::endl;
                    std::cout << "strategy: " << divide_line << " on " << (strategy == DividingStrategy::X ? 'X' : (strategy == DividingStrategy::Y ? 'Y' : 'Z'))
                    << "-axis, with area (" << left_area << '|' << right_area << ')' << std::endl;
                }
                else
                {
                    // create left leaf child box
                    unprocessed_boxes.emplace(vertices, faces, face_start, index_right_start - face_start);
                    box_node.index_l = processed_count - 1 + unprocessed_boxes.size();

                    // create right leaf child box
                    unprocessed_boxes.emplace(vertices, faces, index_right_start, face_stop - index_right_start);
                    box_node.index_r = processed_count - 1 + unprocessed_boxes.size();
                }
            }

            _boxes.push_back(box_node); unprocessed_boxes.pop();
            processed_count++;
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

        std::cout << "# of triangles: " << _triangles.size() << ", # of boxes: " << _boxes.size() << std::endl;

        return prepared = true;
    }


    /******** trace ray ********/

    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        bool hit = false;
        auto [time_in, time_out] = _boxes.front().box.trace(ray);
        rec.box_count++;
        if ((time_out < defaults<fg>::eps) || (time_in >= time_out)) { return hit; }

        std::stack<std::tuple<BoxNode const&, fg>> to_trace_boxes;
        to_trace_boxes.emplace(_boxes.front(), time_in);

        while (!to_trace_boxes.empty())
        {
            auto [box_node, time_in] = to_trace_boxes.top();
            to_trace_boxes.pop();

            if (time_in >= rec.max_ray_time) { continue; }

            if (box_node.isleaf())
            {
                Triangle const* triangle_p = &_triangles[box_node.triangle_start()];
                Triangle const* stop = triangle_p + box_node.triangles_length();
                for (; triangle_p < stop; triangle_p++)
                {
                    rec.triangle_count++;
                    if (triangle_p->trace(ray, rec))
                    {
                        hit = true;
                        rec.mesh_p = this;
                    }
                }
            }
            else
            {
                // trace two chidren box
                BoxNode const* child_l = &_boxes[box_node.leftchild()];
                BoxNode const* child_r = &_boxes[box_node.rightchild()];
                auto [in_l, out_l] = child_l->box.trace(ray);
                auto [in_r, out_r] = child_r->box.trace(ray);
                rec.box_count += 2;

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
                    to_trace_boxes.emplace(*child_r, in_r);
                }
                if ((out_l >= defaults<fg>::eps) && (in_l < out_l))
                {
                    to_trace_boxes.emplace(*child_l, in_l);
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
