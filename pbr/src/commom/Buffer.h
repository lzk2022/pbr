#pragma once
#ifndef __BUFFER_H__
#define __BUFFER_H__
#include <glad/glad.h>
#include <memory>
#include "Mesh.h"
struct MeshBuffer
{
	GLuint vbo;				// 顶点缓冲对象的标识符 Vertex Buffer Object 
	GLuint ibo;				// 索引缓冲对象的标识符 Index Buffer Object
	GLuint vao;				// 顶点数组对象的标识符 Vertex Array Object
	GLuint numElements;		// 元素数量（例如顶点数量或索引数量）
	MeshBuffer() : vbo(0), ibo(0), vao(0) {}
};

struct FrameBuffer
{
	GLuint id;
	GLuint colorTarget;
	GLuint depthStencilTarget;
	int width, height;
	int samples;
	FrameBuffer() : id(0), colorTarget(0), depthStencilTarget(0) {}
};

class Buffer
{
public:
	static MeshBuffer CreateMeshBuffer(const std::shared_ptr<class Mesh>& mesh);
	static void DeleteMeshBuffer(MeshBuffer& buffer);

	static FrameBuffer CreateFrameBuffer(int width, int height, int samples, GLenum colorFormat, GLenum depthstencilFormat);
	static void ResolveFramebuffer(const FrameBuffer& srcfb, const FrameBuffer& dstfb);
	static void DeleteFrameBuffer(FrameBuffer& fb);

	static GLuint CreateUniformBuffer(const void* data, size_t size);
	template<typename T> static GLuint CreateUniformBuffer(const T* data = nullptr)
	{
		return CreateUniformBuffer(data, sizeof(T));
	}
};
#endif // !__BUFFER_H__
