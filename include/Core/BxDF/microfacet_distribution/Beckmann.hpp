#include "microfacet_distribution.hpp"

struct Beckmann : public microfacet_distribution
{
private:
    const f32 alpha_x, alpha_y;

public:
    Beckmann(f32 alpha_x, f32 alpha_y);

    virtual f32 D(const Vector3f& wh) const override;

    virtual f32 lambda(const Vector3f& w) const override;

    virtual std::tuple<Spectrum, Vector3f, f32>
    sample_f(const Vector3f& wi, const Point2f& u) const override;

    f32 PDF(const Vector3f& wi, const Vector3f& wh) const override;
};
