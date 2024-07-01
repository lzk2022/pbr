#include <stdexcept>
#include <stb/stb_image.h>

#include "Image.h"
#include "Path.h"

Image::Image()
	: mWidth(0)
	, mHeight(0)
	, mChannels(0)
	, mIsHDR(false)
{}

std::shared_ptr<Image> Image::ReadFile(const std::string& filename1, int channels)
{
	std::string filename = PATH + filename1;
	LOG_INFO("Loading image: " + filename);

	std::shared_ptr<Image> image{new Image};

	if(stbi_is_hdr(filename.c_str())) 
	{
		float* pixels = stbi_loadf(filename.c_str(), &image->mWidth, &image->mHeight, &image->mChannels, channels);
		if(pixels) 
		{
			image->mPixels.reset(reinterpret_cast<unsigned char*>(pixels));
			image->mIsHDR = true;
		}
	}
	else 
	{
		unsigned char* pixels = stbi_load(filename.c_str(), &image->mWidth, &image->mHeight, &image->mChannels, channels);
		if(pixels) 
		{
			image->mPixels.reset(pixels);
			image->mIsHDR = false;
		}
	}
	if(channels > 0) 
	{
		image->mChannels = channels;
	}
	LOG_ASSERT(!image->mPixels, "Failed to load image file: " + filename);
	return image;
}
