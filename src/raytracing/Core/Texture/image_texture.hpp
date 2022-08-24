#pragma once

#include <math/Vector3.hpp>

#include "Texture.hpp"
#include <util/Image.hpp>
#include <raytracing/hit_record.hpp>

using Spectrum = Vector3f;

struct image_texture : public Texture<Spectrum>
{
private:
    Image image;

public:
    image_texture() = delete;
    image_texture(const Image&) = delete;

    image_texture(Image&& image) : image(std::move(image)) {}

    virtual Spectrum evaluate(const hit_record& record) const
    {
        const auto [width, height] = image.get_width_height();
        const auto w = static_cast<f32>(width - 1);
        const auto h = static_cast<f32>(height - 1);
        const usize u = clamp(ZERO<usize>, static_cast<usize>(w * record.uv.x), width - 1);
        const usize v = clamp(ZERO<usize>, static_cast<usize>(h * record.uv.y), height - 1);
        return rgb_to_float<f32>(image[u][v]);
    }
};
