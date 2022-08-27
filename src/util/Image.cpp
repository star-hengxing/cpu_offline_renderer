#include <string_view>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Image.hpp"
#include "range.hpp"

std::optional<Image> load(const char* path)
{
    int w, h, comp;
    u8* data = stbi_load(path, &w, &h, &comp, 0);
    if (!data)
    {
        return {};
    }

    Image image(w, h);

    if (comp == 3)
    {
        for (auto&& [index, color] : enumerate(image, 3))
        {
            color = Color{data[index], data[index + 1], data[index + 2]};
        }
    }
    else if (comp == 4)
    {
        std::memcpy(image.raw(), data, w * h * 4);
    }
    else
    {
        return {};
    }

    stbi_image_free(data);
    return image;
}

bool save(const Image& image, const char* path)
{
    const auto [width, height] = image.get_width_height();
    auto data = (const u8*)const_cast<Image&>(image).raw();
    return save(path, data, width, height);
}

bool save(const char* path, const u8* data, usize width, usize height)
{
    const auto name = std::string_view{path};
    const auto w = static_cast<int>(width);
    const auto h = static_cast<int>(height);

    if (name.ends_with(".jpg"))
    {
        stbi_write_jpg(path, w, h, 4, data, 0);
    }
    else if (name.ends_with(".png"))
    {
        stbi_write_png(path, w, h, 4, data, 0);
    }
    else if (name.ends_with(".ppm"))
    {
        // P3 format write file by string, P6 format by binary
        FILE* fp = fopen(path, "wb");
        if (!fp)
        {
            return false;
        }
        fprintf(fp, "P6\n%zu %zu\n255\n", width, height);
        for (usize i : range(width * height))
        {
            fwrite(&data[i], 1, 3, fp);
        }
        fclose(fp);
    }
    else
    {
        return false;
    }

    return true;
}
