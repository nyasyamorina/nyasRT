#pragma once

#include <fstream>
#include <filesystem>
#include <math.h>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "../common.hpp"
#include "BoundingBox.hpp"
#include "Ray.hpp"
#include "Transform.hpp"


namespace nyasRT
{
class VertexInfo
{
public:

    vec3g position;
    normal3g normal;
    vec2g texture_coordinate;
};


class Mesh;

using MeshPtr = std::shared_ptr<Mesh>;
using MeshConstPtr = std::shared_ptr<Mesh const>;


class Mesh
{
    using indices_t = glm::vec<3, u32>;

    // interpolation on triangle face
    template<class T> static inline T trinterpolate(T const* data, indices_t const& vertex_indices, vec2g const& uv) noexcept
    {
        return (1 - uv.x - uv.y) * data[vertex_indices.x] + uv.x * data[vertex_indices.y] + uv.y * data[vertex_indices.z];
    }

protected:

    class BoxNode
    {
    public:

        BoundingBox box;
        u32 index_l; // the index to left child node or to triangle if is leaf
        u32 index_r; // the index to right child node or the length of triangles if is leaf, and the highest bit indicate this is leaf or not

        static constexpr u32 leafbit = 0x80000000;

        VEC_CONSTEXPR BoxNode() noexcept
        : box{}, index_l{0}, index_r{0} {}
        VEC_CONSTEXPR BoxNode(vec3g const* vertices, indices_t const* faces, u32 face_start, u32 n_faces) noexcept
        : box{}, index_l{face_start}, index_r{n_faces | leafbit} {
            indices_t const* vertex_indices_p = &faces[face_start];
            indices_t const* stop = vertex_indices_p + n_faces;
            for (; vertex_indices_p < stop; vertex_indices_p++)
            {
                box.bound(vertices[vertex_indices_p->x]);
                box.bound(vertices[vertex_indices_p->y]);
                box.bound(vertices[vertex_indices_p->z]);
            }
            box.prepare();
        }

        constexpr bool isleaf() const noexcept
        {
            return (index_r & leafbit) != 0;
        }

        constexpr u32 leftchild() const noexcept
        {
            return index_l;
        }
        constexpr u32 rightchild() const noexcept
        {
            return index_r;
        }
        constexpr u32 triangle_start() const noexcept
        {
            return index_l;
        }
        constexpr u32 triangles_length() const noexcept
        {
            return index_r & (~leafbit);
        }
    };


    static constexpr u32 max_triangles_per_box = 10;
    static constexpr u32 max_boxes_depth = 32;
    static constexpr u32 max_strategy_iterations = 32;

    enum class DividingStrategy { X, Y, Z };

    void _build_bounding_volume_hierarchy()
    {
        // pre-calculate some useful informations
        std::vector<std::tuple<vec3g, fg>> center_areas;
        center_areas.reserve(_faces.size());
        for (indices_t vertex_indices : _faces)
        {
            vec3g const& A = _vertices[vertex_indices.x];
            vec3g const& B = _vertices[vertex_indices.y];
            vec3g const& C = _vertices[vertex_indices.z];

            // old, correct behavior: ((B - A) + (C - A)).mul(consts<fg>::third).add(A)
            vec3g center = consts<fg>::third * (A + B + C);
            fg area = consts<fg>::half * length(cross(B - A, C - A));

            center_areas.emplace_back(center, area);
        }

        _boxes.clear();
        std::vector<u32> boxes_depth;
        // global box
        _boxes.emplace_back(_vertices.data(), _faces.data(), 0, _faces.size());
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
                    fg min_distance = consts<fg>::inf, delta = box_size.x / (1 << (iter + 1));
                    divide_line += delta * (left_area > right_area ? -1 : 1);
                    // calculate how many area in each side
                    left_area = right_area = 0;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        auto const& info = center_areas[index];
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
                    fg min_distance = consts<fg>::inf, delta = box_size.y / (1 << (iter + 1));
                    d_l += delta * (l_a > r_a ? -1 : 1);
                    // calculate how many area in each side
                    l_a = r_a = 0;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        auto const& info = center_areas[index];
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
                    fg min_distance = consts<fg>::inf, delta = box_size.z / (1 << (iter + 1));
                    d_l += delta * (l_a > r_a ? -1 : 1);
                    // calculate how many area in each side
                    l_a = r_a = 0;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        auto const& info = center_areas[index];
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
                if ((left_area > consts<fg>::eps) && (right_area > consts<fg>::eps))
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
                    u32 right_start = invalid_index;
                    for (u32 index = face_start; index < face_stop; index++)
                    {
                        if (strategy_axis(std::get<0>(center_areas[index])) < divide_line)
                        {
                            if (right_start != invalid_index)
                            {
                                std::swap(       _faces[right_start],      _faces[index]);
                                std::swap(_face_normals[right_start], _face_normals[index]);
                                std::swap( _face_consts[right_start],  _face_consts[index]);
                                std::swap( center_areas[right_start],  center_areas[index]);
                                right_start++;
                            }
                        }
                        else if (right_start == invalid_index)
                        {
                            right_start = index;
                        }
                    }

                    // contruct child-boxes
                    box_node.index_l = box_count++;
                    box_node.index_r = box_count++;
                    boxes_depth.emplace_back(box_depth + 1);
                    boxes_depth.emplace_back(box_depth + 1);
                    _boxes.emplace_back(_vertices.data(), _faces.data(), face_start, right_start - face_start);
                    _boxes.emplace_back(_vertices.data(), _faces.data(), right_start, face_stop - right_start);
                }
            }
        }
    }

    std::vector<BoxNode> _boxes;
    std::vector<vec3g> _vertices;
    std::vector<vec3g> _vertex_normals;
    std::vector<vec2g> _vertex_uv;  // texture coordinate
    std::vector<indices_t>  _faces;
    std::vector<normal3g>   _face_normals;
    std::vector<vec3g>      _face_consts;

public:

    bool enable_normal_interpolation;
    bool custom_vertex_normals; // if true, `vertex_normals` must be set by user, otherwise the behavior is undefined
    bool prepared;

    Mesh() noexcept
    :  enable_normal_interpolation{false}, custom_vertex_normals{false}, prepared{false} {}


    bool prepare()
    {
        if (prepared) { return true; }

        _face_normals.resize(_faces.size());
        _face_consts.resize(_faces.size());

        bool build_vertex_normals = enable_normal_interpolation && !custom_vertex_normals;
        if (build_vertex_normals) for (normal3g & normal : _vertex_normals) { normal = consts<vec3g>::O; }

        u64 const nv = _vertices.size();
        for (u32 face_index = 0; face_index < _faces.size(); face_index++)
        {
            indices_t const& vertex_indices = _faces[face_index];
            normal3g & face_normal = _face_normals[face_index];
            vec3g & face_constants = _face_consts[face_index];

            // calculate face normal and some canstants
            if (vertex_indices.x < nv && vertex_indices.y < nv && vertex_indices.z < nv)
            {
                vec3g const& A = _vertices[vertex_indices.x];
                vec3g const& B = _vertices[vertex_indices.y];
                vec3g const& C = _vertices[vertex_indices.z];

                face_normal = normalize(cross(B - A, C - A));

                face_constants.x = dot(B - A, B - A);
                face_constants.y = dot(B - A, C - A);
                face_constants.z = dot(C - A, C - A);
                fg inv_det = 1 / (face_constants.x * face_constants.z - sqr(face_constants.y));
                face_constants *= inv_det;
            }
            else { return false; }      // vertex index out of range
            if (build_vertex_normals)
            {
                _vertex_normals[vertex_indices.x] += face_normal;
                _vertex_normals[vertex_indices.y] += face_normal;
                _vertex_normals[vertex_indices.z] += face_normal;
            }
        }
        if (enable_normal_interpolation) for (normal3g & normal : _vertex_normals) { normal = normalize(normal); }

        _build_bounding_volume_hierarchy();

#ifdef NYASRT_SHOW_TRACE_INFO
        std::cout << "# of triangles: " << _faces.size() << ", # of boxes: " << _boxes.size() << std::endl;
#endif
        return prepared = true;
    }


    u32 add_vertex(vec3g const& vertex) noexcept
    {
        _vertices      .push_back(vertex);
        _vertex_normals.push_back(consts<vec3g>::Z);
        _vertex_uv     .push_back(consts<vec2g>::O);
        return _vertices.size() - 1;
    }
    u32 add_vertex(VertexInfo const& vertex) noexcept
    {
        _vertices      .push_back(vertex.position);
        _vertex_normals.push_back(vertex.normal);
        _vertex_uv     .push_back(vertex.texture_coordinate);
        return _vertices.size() - 1;
    }

    u32 add_face(u32 A_index, u32 B_index, u32 C_index) noexcept
    {
        return add_face(indices_t(A_index, B_index, C_index));
    }
    u32 add_face(indices_t const& vertex_indices) noexcept
    {
        _faces.push_back(vertex_indices);
        return _faces.size() - 1;
    }

    Mesh & vertex(u32 index, vec3g const& vertex) noexcept
    {
        _vertices[index] = vertex;
        return *this;
    }
    Mesh & vertex(u32 index, VertexInfo const& vertex) noexcept
    {
        _vertices      [index] = vertex.position;
        _vertex_normals[index] = vertex.normal;
        _vertex_uv     [index] = vertex.texture_coordinate;
        return *this;
    }

    Mesh & face(u32 index, u32 A_index, u32 B_index, u32 C_index) noexcept
    {
        return face(index, indices_t(A_index, B_index, C_index));
    }
    Mesh & face(u32 index, indices_t const& vertex_indices) noexcept
    {
        _faces[index] = vertex_indices;
        return *this;
    }

    Mesh & reserve_vertices(u32 n) noexcept
    {
        _vertices      .reserve(n);
        _vertex_normals.reserve(n);
        _vertex_uv     .reserve(n);
        return *this;
    }
    Mesh & reserve_faces(u32 n) noexcept
    {
        _faces.reserve(n);
        return *this;
    }

    VertexInfo vertex(u32 index) const noexcept
    {
        VertexInfo info;
        info.position           = _vertices[index];
        info.normal             = _vertex_normals[index];
        info.texture_coordinate = _vertex_uv[index];
        return info;
    }
    indices_t face(u32 index) const noexcept
    {
        return _faces[index];
    }

    /******** mesh trasformations ********/

    Mesh & project_to_sphere(fg radius = 1) noexcept
    {
        prepared = false;
        for (vec3g & vertex : _vertices)
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
        std::vector<vec3g> new_vertices;
        std::vector<vec3g> new_vertex_normals;
        std::vector<vec2g> new_vertex_uv;  // texture coordinate
        std::vector<indices_t> new_faces;
        new_vertices.reserve(_faces.size() * (((subdivision + 1) * (subdivision + 2)) >> 1));
        new_vertex_normals.reserve(new_vertices.size());
        new_vertex_uv.reserve(new_vertices.size());
        new_faces.reserve(_faces.size() * sqr(subdivision));
        // TODO: vertex normals

        u32 v_index = 0;
        for (indices_t const& vertex_indices : _faces)
        {
            for (u32 v_i = 0; v_i <= subdivision; v_i++)
            {
                for (u32 u_i = 0; u_i <= subdivision - v_i; u_i++)
                {
                    vec2g uv = vec2g(u_i, v_i) / fg(subdivision);
                    new_vertices      .push_back(trinterpolate(_vertices      .data(), vertex_indices, uv));
                    new_vertex_normals.push_back(trinterpolate(_vertex_normals.data(), vertex_indices, uv));
                    new_vertex_uv     .push_back(trinterpolate(_vertex_uv     .data(), vertex_indices, uv));
                }
            }

            for (u32 v_i = 0; v_i < subdivision; v_i++)
            {
                new_faces.emplace_back(v_index, v_index + 1, v_index + (subdivision - v_i));
                v_index++;
                for (u32 u_i = 1; u_i < subdivision - v_i; u_i++)
                {
                    new_faces.emplace_back(v_index, v_index + 1, v_index + (subdivision - v_i));
                    new_faces.emplace_back(v_index, v_index + (subdivision - v_i), (v_index - 1) + (subdivision - v_i));
                    v_index++;
                }
                v_index++;
            }
            v_index++;
        }

        _vertices      .swap(new_vertices);
        _vertex_normals.swap(new_vertex_normals);
        _vertex_uv     .swap(new_vertex_uv);
        _faces.swap(new_faces);
        return *this;
    }


    /******** trace ray ********/

    bool trace_face(u32 face_index, Ray const& ray, TraceRecord & rec)  const noexcept
    {
        indices_t const& vertex_indices = _faces[face_index];
        normal3g const& face_normal = _face_normals[face_index];
        vec3g const& face_constants = _face_consts[face_index];
        vec3g const& A = _vertices[vertex_indices.x];
        vec3g const& B = _vertices[vertex_indices.y];
        vec3g const& C = _vertices[vertex_indices.z];

        fg d_dot_n = dot(ray.direction, face_normal);
        // triangle cannot be seen from behind, may be deprecated this line
        if (d_dot_n >= 0) { return false; }

        fg hit_time = dot(A - ray.origin, face_normal) / d_dot_n;
        if ((hit_time < consts<fg>::eps) || (hit_time >= rec.max_ray_time)) { return false; }

        vec3g hit_point = ray.at(hit_time);
        vec3g coord_point = hit_point - A;
        fg co_u = dot(coord_point, B - A), co_v = dot(coord_point, C - A);
        fg contra_u = co_u * face_constants.z - co_v * face_constants.y;
        fg contra_v = co_v * face_constants.x - co_u * face_constants.y;
        fg contra_w = 1 - contra_u - contra_v;
        if ((contra_u <= 0) || (contra_v <= 0) || (contra_w <= 0)) { return false; }

        rec.max_ray_time = hit_time;
        rec.hit_point = hit_point;
        rec.face_normal = face_normal;
        rec.hit_normal = face_normal;
        rec.hit_face = vec2g(contra_u, contra_v);
        rec.hit_texture = trinterpolate(_vertex_uv.data(), vertex_indices, rec.hit_face);
        if (enable_normal_interpolation)
        {
            rec.hit_normal = trinterpolate(_vertex_normals.data(), vertex_indices, rec.hit_face);
        }
        return true;
    }

    bool trace(Ray const& ray, TraceRecord & rec) const noexcept
    {
        auto [time_in, time_out] = _boxes.front().box.trace(ray);
#ifdef NYASRT_SHOW_TRACE_INFO
        rec.box_count++;
#endif
        if ((time_out < consts<fg>::eps) || (time_in >= time_out)) { return false; }

        using StackEltype = std::tuple<BoxNode const* /* box */, fg /* time_in */>;
        StackEltype to_trace_boxes[max_boxes_depth + 1];
        StackEltype * box_p = to_trace_boxes;
        *box_p = {&_boxes.front(), time_in};

        bool hit = false;
        while (box_p >= to_trace_boxes)
        {
            BoxNode const& box_node = *std::get<0>(*box_p);
            time_in = std::get<1>(*(box_p--));

            if (time_in >= rec.max_ray_time) { continue; }

            if (box_node.isleaf())
            {
                u32 stop = box_node.triangle_start() + box_node.triangles_length();
                for (u32 face_index = box_node.triangle_start(); face_index < stop; face_index++)
                {
                    hit |= trace_face(face_index, ray, rec);
#ifdef NYASRT_SHOW_TRACE_INFO
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
#ifdef NYASRT_SHOW_TRACE_INFO
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
                if ((out_r >= consts<fg>::eps) && (in_r < out_r))
                {
                    *(++box_p) = {child_r, in_r};
                }
                if ((out_l >= consts<fg>::eps) && (in_l < out_l))
                {
                    *(++box_p) = {child_l, in_l};
                }
            }
        }
        return hit;
    }


    bool test_hit_face(u32 face_index, Ray const& ray, fg max_ray_time) const noexcept
    {
        indices_t const& vertex_indices = _faces[face_index];
        normal3g const& face_normal = _face_normals[face_index];
        vec3g const& face_constants = _face_consts[face_index];
        vec3g const& A = _vertices[vertex_indices.x];
        vec3g const& B = _vertices[vertex_indices.y];
        vec3g const& C = _vertices[vertex_indices.z];

        fg d_dot_n = dot(ray.direction, face_normal);
        // triangle cannot be seen from behind, may be deprecated this line
        if (d_dot_n >= 0) { return false; }

        fg hit_time = dot(A - ray.origin, face_normal) / d_dot_n;
        if ((hit_time < consts<fg>::eps) || (hit_time >= max_ray_time)) { return false; }

        vec3g hit_point = ray.at(hit_time);
        vec3g coord_point = hit_point - A;
        fg co_u = dot(coord_point, B - A), co_v = dot(coord_point, C - A);
        fg contra_u = co_u * face_constants.z - co_v * face_constants.y;
        fg contra_v = co_v * face_constants.x - co_u * face_constants.y;
        fg contra_w = 1 - contra_u - contra_v;
        if ((contra_u <= 0) || (contra_v <= 0) || (contra_w <= 0)) { return false; }

        return true;
    }

    bool test_hit(Ray const& ray, fg max_ray_time) const noexcept
    {
        auto [time_in, time_out] = _boxes.front().box.trace(ray);
        if ((time_out < consts<fg>::eps) || (time_in >= time_out)) { return false; }

        using StackEltype = std::tuple<BoxNode const* /* box */, fg /* time_in */>;
        StackEltype to_trace_boxes[max_boxes_depth + 1];
        StackEltype * box_p = to_trace_boxes;
        *box_p = {&_boxes.front(), time_in};

        while (box_p >= to_trace_boxes)
        {
            BoxNode const& box_node = *std::get<0>(*box_p);
            time_in = std::get<1>(*(box_p--));

            if (time_in >= max_ray_time) { continue; }

            if (box_node.isleaf())
            {
                u32 stop = box_node.triangle_start() + box_node.triangles_length();
                for (u32 face_index = box_node.triangle_start(); face_index < stop; face_index++)
                {
                    if (test_hit_face(face_index, ray, max_ray_time)) { return true; }
                }
            }
            else
            {
                // trace two chidren box
                BoxNode const* child_l = &_boxes[box_node.leftchild()];
                BoxNode const* child_r = &_boxes[box_node.rightchild()];
                auto [in_l, out_l] = child_l->box.trace(ray);
                auto [in_r, out_r] = child_r->box.trace(ray);

                // push them in to stack (or not)
                if ((out_r >= consts<fg>::eps) && (in_r < out_r))
                {
                    *(++box_p) = {child_r, in_r};
                }
                if ((out_l >= consts<fg>::eps) && (in_l < out_l))
                {
                    *(++box_p) = {child_l, in_l};
                }
            }
        }
        return false;
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
    static MeshPtr torus(fg thickness, u32 n_a, u32 n_b);
};


MeshPtr Mesh::load_obj(std::ifstream & file)
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    if (!file.is_open()) { return mesh_p; }

    vec3g v; indices_t v_indeces;

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
            mesh_p->add_vertex(v);
        }
        else if (part == "f")   // polygonal face element
        {
            line_buff >> part; v_indeces.x = s2n<u32>(part);
            line_buff >> part; v_indeces.y = s2n<u32>(part);
            line_buff >> part; v_indeces.z = s2n<u32>(part);
            // TODO: support many indces `.../.../...`
            // TODO: slpit polygon to triangles
            mesh_p->add_face(v_indeces - indices_t(1));
        }
    }

    return mesh_p;
}

MeshPtr Mesh::tetrahedron()
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->reserve_vertices(4).reserve_faces(4);

    f32 root2 = std::sqrt(2.0f);
    f32 z = -consts<f32>::third;
    f32 x1 = 2 * root2 / 3;
    f32 x2 = -root2 / 3;
    f32 y = std::sqrt(2.0f / 3.0f);

    u32 top               = mesh_p->add_vertex(vec3g(0,   0, 1));
    u32 bottom_front      = mesh_p->add_vertex(vec3g(x1,  0, z));
    u32 bottom_back_left  = mesh_p->add_vertex(vec3g(x2,  y, z));
    u32 bottom_back_right = mesh_p->add_vertex(vec3g(x2, -y, z));

    mesh_p->add_face(top, bottom_back_right, bottom_front);
    mesh_p->add_face(top, bottom_front, bottom_back_left);
    mesh_p->add_face(top, bottom_back_left, bottom_back_right);
    mesh_p->add_face(bottom_front, bottom_back_right, bottom_back_left);

    mesh_p->enable_normal_interpolation = false;
    return mesh_p;
}

MeshPtr Mesh::hexahedron()
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->reserve_vertices(8).reserve_faces(12);

    f32 v = 1.0f / std::sqrt(3.0f);

    mesh_p->add_vertex(vec3g(-v, -v, -v));
    mesh_p->add_vertex(vec3g( v, -v, -v));
    mesh_p->add_vertex(vec3g( v,  v, -v));
    mesh_p->add_vertex(vec3g(-v,  v, -v));
    mesh_p->add_vertex(vec3g(-v, -v,  v));
    mesh_p->add_vertex(vec3g( v, -v,  v));
    mesh_p->add_vertex(vec3g( v,  v,  v));
    mesh_p->add_vertex(vec3g(-v,  v,  v));

    mesh_p->add_face(0, 3, 2);
    mesh_p->add_face(0, 2, 1);
    mesh_p->add_face(0, 1, 5);
    mesh_p->add_face(0, 5, 4);
    mesh_p->add_face(1, 2, 6);
    mesh_p->add_face(1, 6, 5);
    mesh_p->add_face(2, 3, 7);
    mesh_p->add_face(2, 7, 6);
    mesh_p->add_face(3, 0, 4);
    mesh_p->add_face(3, 4, 7);
    mesh_p->add_face(4, 5, 7);
    mesh_p->add_face(5, 6, 7);

    mesh_p->enable_normal_interpolation = false;
    return mesh_p;
}

MeshPtr Mesh::octahedron()
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->reserve_vertices(6).reserve_faces(8);

    mesh_p->add_vertex(vec3g( 0,  0,  1));
    mesh_p->add_vertex(vec3g( 1,  0,  0));
    mesh_p->add_vertex(vec3g( 0,  1,  0));
    mesh_p->add_vertex(vec3g(-1,  0,  0));
    mesh_p->add_vertex(vec3g( 0, -1,  0));
    mesh_p->add_vertex(vec3g( 0,  0, -1));

    mesh_p->add_face(0, 1, 2);
    mesh_p->add_face(0, 2, 3);
    mesh_p->add_face(0, 3, 4);
    mesh_p->add_face(0, 4, 1);
    mesh_p->add_face(5, 2, 1);
    mesh_p->add_face(5, 3, 2);
    mesh_p->add_face(5, 4, 3);
    mesh_p->add_face(5, 1, 4);

    mesh_p->enable_normal_interpolation = false;
    return mesh_p;
}

// TODO: MeshPtr Mesh::dodecahedron()

// TODO: MeshPtr Mesh::icosahedron()

MeshPtr Mesh::uv_sphere(u32 n_longitude, u32 n_latitude)
{
    n_longitude = std::max(2u, n_longitude);
    n_latitude  = std::max(1u, n_latitude);

    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->reserve_vertices(n_longitude * n_latitude * 2).reserve_faces(n_longitude * n_latitude + 2);

    mesh_p->add_vertex(VertexInfo{ consts<vec3g>::Z,  consts<vec3g>::Z, consts<vec2g>::Y});    // top vertex
    mesh_p->add_vertex(VertexInfo{-consts<vec3g>::Z, -consts<vec3g>::Z, consts<vec2g>::O});    // bottom vertex
    for (u32 v = 0; v < n_latitude; v++)
    {
        fg theta = consts<fg>::pi * (v + 1) / (n_latitude + 1);
        fg stheta = std::sin(theta), ctheta = std::cos(theta);
        for (u32 u = 0; u < n_longitude; u++)
        {
            fg phi = consts<fg>::two_pi * u / n_longitude;
            fg sphi = std::sin(phi), cphi = std::cos(phi);

            VertexInfo vertex;
            vertex.position = vec3g(cphi * stheta, sphi * stheta, ctheta);
            vertex.normal = vertex.position;
            vertex.texture_coordinate = vec2g(u, v + 1) / vec2g(n_longitude - 1, n_latitude + 1);
            mesh_p->add_vertex(vertex);
        }
    }

    u32 v_index = (n_latitude - 1) * n_longitude + 2;
    for (u32 u = 1; u < n_longitude; u++)
    {
        mesh_p->add_face(0, u + 1, u + 2);    // top faces
        mesh_p->add_face(1, v_index + u, v_index + u - 1);    // bottom faces
    }
    mesh_p->add_face(0, n_longitude + 1, 2);  // top face when u = 0
    mesh_p->add_face(1, v_index, v_index + n_longitude - 1);  // bottom face when u = 0
    for (u32 v = 1; v < n_latitude; v++)
    {
        v_index = n_longitude * v + 2;
        for (u32 u = 1; u < n_longitude; u++)
        {
            // rectangle
            mesh_p->add_face(v_index + u - 1, v_index + u, v_index - n_longitude + u - 1);
            mesh_p->add_face(v_index + u, v_index - n_longitude + u, v_index - n_longitude + u - 1);
        }
        // rectangle when u = 0
        mesh_p->add_face(v_index + n_longitude - 1, v_index, v_index - 1);
        mesh_p->add_face(v_index, v_index - n_longitude, v_index - 1);
    }

    mesh_p->enable_normal_interpolation = true;
    mesh_p->custom_vertex_normals = true;
    return mesh_p;
}

MeshPtr Mesh::torus(fg tube_radius, u32 n_a, u32 n_b)
{
    MeshPtr mesh_p = std::make_shared<Mesh>();
    mesh_p->reserve_vertices((n_a + 1) * (n_b + 1)).reserve_faces(n_a * n_b * 2);

    VertexInfo vertex;
    for (u32 b = 0; b <= n_b; b++)
    {
        fg theta = consts<fg>::two_pi * b / n_b;
        fg xx = std::cos(theta), yy = std::sin(theta);

        for (u32 a = 0; a <= n_a; a++)
        {
            fg phi = consts<fg>::two_pi * a / n_a;
            fg x = std::cos(phi), z = std::sin(phi);

            vertex.position = vec3g((tube_radius * x + 1) * xx, (tube_radius * x + 1) * yy, tube_radius * z);
            vertex.normal = vec3g(x * xx, x * yy, z);
            vertex.texture_coordinate = vec2g(b, a) / vec2g(n_b, n_a);
            mesh_p->add_vertex(vertex);

            if ((b < n_b) && (a < n_a))
            {
                mesh_p->add_face(b * (n_a + 1) + a, (b + 1) * (n_a + 1) + a, (b + 1) * (n_a + 1) + (a + 1));
                mesh_p->add_face(b * (n_a + 1) + a, (b + 1) * (n_a + 1) + (a + 1), b * (n_a + 1) + (a + 1));
            }
        }
    }

    mesh_p->enable_normal_interpolation = true;
    mesh_p->custom_vertex_normals = true;
    return mesh_p;
}

} // namespace nyasRT
