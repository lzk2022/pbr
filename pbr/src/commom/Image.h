#pragma once
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <cassert>
#include <memory>
#include <string>

class Image
{
public:
	static std::shared_ptr<Image> ReadFile(const std::string& filename, int channels=4);
	template<typename T>
	const T* GetPixels() const
	{
		return reinterpret_cast<const T*>(mPixels.get());
	}

public:
	int mWidth;
	int mHeight;
	int mChannels;
	bool mIsHDR;

private:
	Image();
	std::unique_ptr<unsigned char> mPixels;
};

#endif // !__IMAGE_H__