#pragma once

#include <optional>
#include <vector>
#include <memory>
#include <array>

#include <Hinae/Bounds3.hpp>
#include <Hinae/Ray3.hpp>

using namespace Hinae;

#include <hit_record.hpp>

struct geometry_primitive;

template <typename T>
struct MemoryPool
{
private:
    std::unique_ptr<T[]> pool;
    usize size;
    usize index;

public:
    MemoryPool(usize size)
        : pool(std::make_unique<T[]>(size)), size(size), index(0) {}
    
    T* alloc()
    {
        if(index >= size)
        {
            return nullptr;
        }
        else
        {
            index++;
            return &pool[index - 1];
        }
    }
};

struct BVH
{
    enum class split_method { native };

    struct Node
    {
        Bounds3f bounds;
        Node* left;
        Node* right;
        Axis axis;
        usize primitive_first;
        usize primitive_count;

        Node() = default;

        void init_leaf(const Bounds3f& bounds, usize first, usize count);

        void init_interior(Axis axis, Node* left, Node* right);
    };

    struct flat_array_node
    {
        Bounds3f bounds;
        Axis axis;
        union
        {
            usize primitive_first;
            usize right_index;
        };
        usize primitive_count;
        usize padding[2];

        flat_array_node() = default;
    };

    struct Primitive_info
    {
        usize index;
        Bounds3f bounds;
        Point3f  centroid;

        Primitive_info() = default;
        Primitive_info(usize index, const Bounds3f& bounds);
    };

private:
    usize total_nodes;
    usize total_geometrys;
    std::vector<std::shared_ptr<geometry_primitive>> primitives;
    std::unique_ptr<flat_array_node[]> flat_bvh;

private:
    Node* recursive_build(MemoryPool<Node>& pool
        , std::vector<Primitive_info>& primitive_info
        , usize start, usize end
        , std::vector<std::shared_ptr<geometry_primitive>>& order_primitive);

    std::shared_ptr<geometry_primitive> recursive_intersect(const Ray3f& ray
        , hit_record& record
        , const Vector3f& inv_dir
        , const std::array<bool, 3> dir_is_negative) const;

    usize recursive_flat(Node* node, usize& index);

public:
    BVH() = default;

    BVH(const std::vector<std::shared_ptr<geometry_primitive>>& data);
    
    void build(const std::vector<std::shared_ptr<geometry_primitive>>& data);

    std::optional<hit_record> intersect(const Ray3f& ray) const;

    bool intersect_p(const Ray3f& shadow_ray) const;
};
