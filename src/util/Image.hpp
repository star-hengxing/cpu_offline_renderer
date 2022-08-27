#pragma once

#include <optional>

#include "Color/Color.hpp"
#include "Buffer2D.hpp"

using Image = Buffer2D<Color>;

std::optional<Image> load(const char* path);

bool save(const Image& image, const char* path);
// Support png, jpg, ppm
bool save(const char* path, const u8* data, usize width, usize height);
