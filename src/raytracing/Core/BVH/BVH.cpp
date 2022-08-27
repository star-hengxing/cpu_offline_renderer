#include <raytracing/Core/Primitive/geometry_primitive.hpp>
#include <raytracing/Core/BVH/BVH.hpp>
#include <raytracing/hit_record.hpp>
#include <util/range.hpp>
#include <util/io.hpp>

using Node            = BVH::Node;
using Primitive_info  = BVH::Primitive_info;
using flat_array_node = BVH::flat_array_node;

template <typename T, usize size = 64>
struct Stack
{
private:
    std::array<T, size> stack;
    T top = 0;

public:
    Stack() {};

    void push(T value)
    {
        stack[top] = value;
        top++;
        assert(top < size);
    }

    T pop()
    {
        top--;
        assert(top < size);
        return stack[top];
    }

    bool empty() const
    {
        return top == 0;
    }
};

Primitive_info::Primitive_info(usize index, const Bounds3f& bounds)
    : index(index), bounds(bounds), centroid(bounds.centroid()) {}

void Node::init_leaf(const Bounds3f& bounds, usize index, usize count)
{
    this->bounds          = bounds;
    this->primitive_first = index;
    this->primitive_count = count;
    left = right = nullptr;
}

void Node::init_interior(Axis axis, Node* left, Node* right)
{
    this->axis  = axis;
    this->left  = left;
    this->right = right;
    bounds = Union(left->bounds, right->bounds);
    primitive_count = 0;
}

BVH::BVH(const View<geometry_primitive*>& data)
{
    build(data);
}

void BVH::build(const View<geometry_primitive*>& data)
{
    if (data.empty()) return;
    total_nodes = total_geometrys = 0;

    primitives.reserve(data.size);
    for (auto i : range(data.size))
    {
        primitives[i] = data[i];
    }

    MemoryPool<Node> pool(primitives.size() * 2 - 1);
    std::vector<geometry_primitive*> order_primitive;
    order_primitive.reserve(primitives.size());

    std::vector<Primitive_info> primitive_info(primitives.size());
    for (auto [i, value] : enumerate(primitive_info))
    {
        value = Primitive_info{i, primitives[i]->world_bound()};
    }

    Node* root = recursive_build(pool, primitive_info, 0, primitive_info.size(), order_primitive);

    primitives.swap(order_primitive);
    flat_bvh = std::make_unique<flat_array_node[]>(total_nodes);
    usize index = 0;
    recursive_flat(root, index);
}

Node* BVH::recursive_build(MemoryPool<Node>& pool
    , std::vector<Primitive_info>& primitive_info
    , usize start, usize end
    , std::vector<geometry_primitive*>& order_primitive)
{
    Node* node = pool.alloc();
    if (!node)
    {
        println("out of MemoryPool<BVH::Node>");
        exit(-1);
    }
    total_nodes++;

    Bounds3f bounds = primitive_info[start].bounds;
    for (auto i : range(start + 1, end))
    {
        bounds = Union(bounds, primitive_info[i].bounds);
    }

    usize size = end - start;
    if (size == 1)
    {
        order_primitive.push_back(primitives[primitive_info[start].index]);
        node->init_leaf(bounds, order_primitive.size() - 1, 1);
        return node;
    }
    else
    {
        assert(!is_zero(size));
        Bounds3f centroid_bounds = primitive_info[start].centroid;
        for (usize i : range(start + 1, end))
        {
            centroid_bounds = Union(centroid_bounds, primitive_info[i].centroid);
        }

        Axis axis = centroid_bounds.max_extent();
        if (centroid_bounds.p_min[axis] != centroid_bounds.p_max[axis])
        {
            usize mid = (start + end) / 2;
            const f32 f_mid = (centroid_bounds.p_min[axis] + centroid_bounds.p_max[axis]) / 2;
            Primitive_info* p_mid = std::partition(&primitive_info[start]
                , &primitive_info[end - 1] + 1
                , [axis, f_mid](const Primitive_info& info)
                {
                    return info.centroid[axis] < f_mid;
                });

            mid = p_mid - &primitive_info[0];
            if (mid == start || mid == end)
            {
                mid = (start + end) / 2;
                std::nth_element(&primitive_info[start]
                    , &primitive_info[mid]
                    , &primitive_info[end - 1] + 1
                    , [axis](const Primitive_info& a, const Primitive_info& b)
                    {
                        return a.centroid[axis] < b.centroid[axis];
                    });
            }

            Node* left  = recursive_build(pool, primitive_info, start, mid, order_primitive);
            Node* right = recursive_build(pool, primitive_info, mid, end, order_primitive);

            node->init_interior(axis, left, right);
            return node;
        }
        else
        {
            const usize first_index = order_primitive.size();
            for (usize i : range(start, end))
            {
                order_primitive.push_back(primitives[primitive_info[i].index]);
            }
            node->init_leaf(bounds, first_index, size);
        }
    }
    return node;
}

usize BVH::recursive_flat(Node* node, usize& index)
{
    flat_array_node* array_node = &flat_bvh[index];
    array_node->bounds = node->bounds;

    usize next = index;
    index++;

    if (node->primitive_count > 0)
    {
        array_node->primitive_first = node->primitive_first;
        array_node->primitive_count = node->primitive_count;
    }
    else
    {
        array_node->axis = node->axis;
        array_node->primitive_count = 0;
        recursive_flat(node->left, index);
        array_node->right_index = recursive_flat(node->right, index);
    }
    return next;
}

isize BVH::recursive_intersect(const Ray3f& ray
        , hit_record& record
        , const Vector3f& inv_dir
        , const std::array<bool, 3> dir_is_negative) const
{
    usize current = 0;
    Stack<usize> stack;
    isize index = without_intersect;
    while(true)
    {
        const flat_array_node* node = &flat_bvh[current];
        if (node->bounds.intersect(ray, inv_dir))
        {
            if (node->primitive_count > 0)
            {
                const auto end = node->primitive_first + node->primitive_count;
                for (usize i : range(node->primitive_first, end))
                {
                    if(primitives[i]->intersect(ray, record))
                        index = i;
                }
                if (stack.empty()) break;
                current = stack.pop();
            }
            else
            {
                if (dir_is_negative[static_cast<usize>(node->axis)])
                {
                    stack.push(current + 1);
                    current = node->right_index;
                }
                else
                {
                    stack.push(node->right_index);
                    current++;
                }
            }
        }
        else
        {
            if (stack.empty()) break;
            current = stack.pop();
        }
    }
    return index;
}

std::optional<hit_record> BVH::intersect(const Ray3f& ray) const
{
    hit_record record;
    const auto [x, y, z] = ray.direction;
    const Vector3f inv_dir{1 / x, 1 / y, 1 / z};
    const std::array<bool, 3> dir_is_negative{x < 0, y < 0, z < 0};

    const auto index = recursive_intersect(ray, record, inv_dir, dir_is_negative);
    if (index == without_intersect) return {};

    primitives[index]->get_intersect_record(ray, record);
    primitives[index]->compute_BxDF(record);
    return record;
}

std::optional<hit_record> BVH::intersect_p(const Ray3f& shadow_ray, f32 t_max) const
{
    hit_record record;
    record.t_max = t_max;
    const auto [x, y, z] = shadow_ray.direction;
    const Vector3f inv_dir{1 / x, 1 / y, 1 / z};
    const std::array<bool, 3> dir_is_negative{x < 0, y < 0, z < 0};
    const auto index = recursive_intersect(shadow_ray, record, inv_dir, dir_is_negative);
    return (index != without_intersect) ? std::make_optional(record) : std::nullopt;
}
