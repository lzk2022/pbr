#pragma once
#ifndef __SHADER_H__
#define __SHADER_H__
#include <initializer_list>
#include <glad/glad.h>
#include <string>
class Shader
{
public:
	static GLuint LinkProgram(std::initializer_list<std::string> shaderFiles);

private:
	static GLuint CompileShader(const std::string& filename, GLenum type);
	static std::string ReadShaderFile(const std::string& filename);
};

#endif // !__SHADER_H__
