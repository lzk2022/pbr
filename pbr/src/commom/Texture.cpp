#include "Texture.h"
#include "Image.h"
#include "Path.h"
//#include <utility>
#include <cmath>

Texture::Texture()
{
}

Texture::~Texture()
{
}

Texture::Texture(GLenum target, int width, int height, GLenum iformat, int levels)
{
	Init(target, width, height, iformat, levels);
}

Texture::Texture(std::string filename, int channel, GLenum format, GLenum iformat, int level)
{
	Init(filename, channel, format, iformat, level);
}

void Texture::Init(GLenum target, int width, int height, GLenum iformat, int level)
{
	mWidth = width;
	mHeight = height;
	// 如果级别为 0，则自动计算所需的 MIPMAP 级别数
	mLevel = (level == 0) ? CalMipmapLevel() : level;
	CreateTexture(target, iformat);
}

void Texture::Init(std::string filename, int channel, GLenum format, GLenum iformat, int level)
{
	std::shared_ptr<Image> image= Image::ReadFile(filename, channel);
	mWidth = image->mWidth;
	mHeight = image->mHeight;

	// 如果级别为 0，则自动计算所需的 MIPMAP 级别数
	mLevel = (level == 0) ? CalMipmapLevel() : level;

	CreateTexture(GL_TEXTURE_2D, iformat);

	if (image->mIsHDR)
	{
		glTextureSubImage2D(mId, 0, 0, 0, mWidth, mHeight, format, GL_FLOAT, image->GetPixels<float>());
	}
	else
	{
		glTextureSubImage2D(mId, 0, 0, 0, mWidth, mHeight, format, GL_UNSIGNED_BYTE, image->GetPixels<unsigned char>());
	}

	if (mLevel > 1)
	{
		glGenerateTextureMipmap(mId);
	}
}

void Texture::DelTexture()
{
	glDeleteTextures(1, &mId);
	mId = 0;
}

void Texture::CreateTexture(GLenum target, GLenum iformat)
{
	glCreateTextures(target, 1, &mId);
	glTextureStorage2D(mId, mLevel, iformat, mWidth, mHeight);
	glTextureParameteri(mId, GL_TEXTURE_MIN_FILTER, mLevel > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTextureParameteri(mId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameterf(mId, GL_TEXTURE_MAX_ANISOTROPY_EXT, gMaxAnisotropy);
}

void Texture::Init()
{

}

int Texture::CalMipmapLevel() const
{
	return 1 + static_cast<GLuint>(floor(std::log2(std::max(mWidth, mHeight))));
}
