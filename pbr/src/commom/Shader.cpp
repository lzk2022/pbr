#include "Shader.h"
#include <memory>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "Log.h"
#include "Path.h"

GLuint Shader::LinkProgram(std::initializer_list<std::string> shaderFiles)
{
	// 添加着色器文件类型映射
	static std::map<std::string, GLenum> shaderType =
	{
		{"vert",GL_VERTEX_SHADER},			// 顶点着色器（Vertex Shader
		{"frag",GL_FRAGMENT_SHADER},		// 片段着色器（Fragment Shader）
		{"geom",GL_COMPUTE_SHADER},			// 几何着色器（Geometry Shader）
		{"comp",GL_COMPUTE_SHADER},			// 计算着色器（Compute Shader）
		{"tesc",GL_TESS_CONTROL_SHADER},	// 控制着色器（Tessellation Control Shader）
		{"tese",GL_TESS_EVALUATION_SHADER}	// 评估着色器（Tessellation Evaluation Shader）
	};

	std::vector<GLuint> shaders;
	shaders.reserve(shaderFiles.size());
	std::string ext = "";
	GLuint shaderId;
	GLuint program = glCreateProgram();
	for (std::string file : shaderFiles)
	{
		ext = file.substr(file.find_last_of(".") + 1);
		file = "shaders\\glsl\\" + file;
		file = PATH + file;
		//LOG_ASSERT(!shaderType[ext], "编译着色器文件错误，文件类型不支持\t"+file);
		LOG_ASSERT(!shaderType[ext], "Wrong compilation shader file, file type not supported:\t" + file);
		shaderId = CompileShader(file, shaderType[ext]);
		glAttachShader(program, shaderId);
		shaders.push_back(shaderId);
	}
	glLinkProgram(program);
	for (GLuint shader : shaders)
	{
		glDetachShader(program, shader);
		glDeleteShader(shader);
	}

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_TRUE) 
	{
		glValidateProgram(program);
		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	}
	if (status != GL_TRUE) 
	{
		GLsizei infoLogSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
		std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogSize]);
		glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog.get());
		LOG_EXCEPTION(std::string("Program link failed\n") + infoLog.get());
	}
	return program;
}

GLuint Shader::CompileShader(const std::string& filename, GLenum type)
{
	const std::string src = ReadShaderFile(filename);
	LOG_ASSERT(src.empty(), "Read shader file failed:\t" + filename);
	LOG_INFO("Compiling GLSL shader: "+filename);
	const GLchar* srcBufferPtr = src.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &srcBufferPtr, nullptr);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) 
	{
		GLsizei infoLogSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
		std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogSize]);
		glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog.get());
		LOG_EXCEPTION(std::string("Shader compilation failed: ") + filename + "\n" + infoLog.get());
	}
	return shader;
}

std::string Shader::ReadShaderFile(const std::string& filename)
{
	std::ifstream file{ filename };
	LOG_ASSERT(!file.is_open(), "Could not open file: " + filename);

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
	return std::string();
}
