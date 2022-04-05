#include "Microface.hpp"

struct trowbridge_reitz : public Microface
{
private:
    const f32 alpha_x, alpha_y;

public:
    trowbridge_reitz(f32 alpha_x, f32 alpha_y);

    virtual f32 D(const Vector3f& wh) const = 0;

    virtual f32 lambda(const Vector3f& w) const = 0;

    virtual Vector3f Sample_f(
        const Vector3f& wi, const Vector3f& n, const Point2f& u) const = 0;

    f32 PDF(const Vector3f& wi, const Vector3f& wh) const;
};
