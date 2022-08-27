#pragma once

#include <iostream>

#include <util/basic_type.hpp>

inline void bar(f64 f)
{
    fprintf(stderr, "Rendering...%d%%\r", static_cast<int>(f * 100));
}

void render(const char* path);

void render_single_image(usize spp, usize thread);
