#include <util/Image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <util/util.hpp>

std::optional<Image> Image::read(const std::string_view& filename)
{
	std::optional<Image> result;
	int w, h, comp;
    u8 *data = stbi_load(filename.data(), &w, &h, &comp, 0);
	if(!data)
		return result;

	Image image(w, h);

	if(comp == 3)
	{
		// std::ranges::for_each(image.get(), [i = usize{}, data](Color& color) mutable
		// {
		// 	i += 3;
		// 	color = Color{data[i - 3], data[i - 2], data[i - 1]};
		// });
		for(auto&& [index, color] : enumerate(image.get(), 3))
		{
			color = Color{data[index], data[index + 1], data[index + 2]};
		}
		result = std::move(image);
	}
	else if(comp == 4)
	{
		std::memcpy(&image.get().data()->r, data, w * h * 4);
		result = std::move(image);
	}
	stbi_image_free(data);
	return result;
}

void Image::write(const std::string_view& filename, Image::Type type, const u8* data, usize width, usize height)
{
	const auto w = static_cast<int>(width);
	const auto h = static_cast<int>(height);
	switch(type)
	{
		using enum Image::Type;
		case JPG:
			stbi_write_jpg(filename.data(), w, h, 4, data, 0);
			break;
		case PNG:
			stbi_write_png(filename.data(), w, h, 4, data, 0);
			break;
		case PPM:
			{
				// P3 format write file by string, P6 format by binary
				FILE* fp = fopen(filename.data(), "wb");
				if(fp)
				{
					fprintf(fp, "P6\n%zu %zu\n255\n", width, height);
					for(usize i : range(width * height))
					{
						fwrite(&data[i], 1, 3, fp);
					}
					fclose(fp);
				}
			}
			break;
		default:
			break;
	}
}

void Image::write(const std::string_view& filename, Type type) const
{
	write(filename, type, &(raw()->r), width, height);
}
