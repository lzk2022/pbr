#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include <glad/glad.h>
#include <string>

class Texture
{
public:
	Texture();
	~Texture();
	Texture(GLenum target, int width, int height, GLenum internalformat, int levels = 0);
	Texture(std::string filename, int channel, GLenum format, GLenum iformat, int level = 0);
	void Init(GLenum target, int width, int height, GLenum internalformat, int levels = 0);
	void Init(std::string filename, int channel, GLenum format, GLenum iformat, int level = 0);

	void DelTexture();

private:
	void CreateTexture(GLenum target, GLenum iformat);
	void Init();
	int  CalMipmapLevel() const;

public:
	GLuint mId;
	int mWidth;
	int mHeight;
	int mLevel;

};
#endif // !__TEXTURE_H__
