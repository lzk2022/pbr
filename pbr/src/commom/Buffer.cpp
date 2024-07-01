#include "Buffer.h"
#include "Log.h"
#include <GLFW/glfw3.h>

MeshBuffer Buffer::CreateMeshBuffer(const std::shared_ptr<class Mesh>& mesh)
{
	MeshBuffer buffer;
	buffer.numElements = static_cast<GLuint>(mesh->mTriangle.size()) * 3;

	const size_t vertexDataSize = mesh->mVertices.size() * sizeof(Mesh::Vertex);
	const size_t indexDataSize = mesh->mTriangle.size() * sizeof(Mesh::Triangle);

	glCreateBuffers(1, &buffer.vbo);
	glNamedBufferStorage(										// 为命名缓冲区分配存储空间
		buffer.vbo,												// buffer.vbo：缓冲区对象的标识符
		vertexDataSize,											// size：数据大小
		reinterpret_cast<const void*>(&mesh->mVertices[0]),		// data：数据指针的转换
		0														// flags：标志位，此处未使用
	);

	glCreateBuffers(1, &buffer.ibo);
	glNamedBufferStorage(buffer.ibo, indexDataSize, reinterpret_cast<const void*>(&mesh->mTriangle[0]), 0);

	glCreateVertexArrays(1, &buffer.vao);
	// 将索引缓冲对象绑定到顶点数组对象，从而关联索引数据到顶点数据
	glVertexArrayElementBuffer(buffer.vao, buffer.ibo);
	for (int i = 0; i < Mesh::mkNumAttributes; ++i) 
	{
		glVertexArrayVertexBuffer(		// 将顶点缓冲绑定到顶点数组对象
			buffer.vao,					// vaobj：顶点数组对象的标识符
			i,							// bindingindex：绑定点索引
			buffer.vbo,					// buffer：顶点缓冲对象的标识符
			i * sizeof(glm::vec3),		// offset：偏移量，以字节为单位
			sizeof(Mesh::Vertex)		// stride：步长，指定每个顶点数据之间的间隔
		);
		glEnableVertexArrayAttrib(		// 启用顶点数组对象的顶点属性
			buffer.vao,					// vaobj：顶点数组对象的标识符
			i							// index：顶点属性索引
		);
		GLint size = i == (Mesh::mkNumAttributes - 1) ? 2 : 3;
		glVertexArrayAttribFormat(		// 设置顶点数组对象的顶点属性格式
			buffer.vao,					// vaobj：顶点数组对象的标识符
			i,							// index：顶点属性索引
			size,						// size：每个顶点属性的组件数量
			GL_FLOAT,					// type：数据类型，此处为浮点型
			GL_FALSE,					// normalized：是否需要数据归一化，此处为否
			0							// relativeoffset：相对偏移量，此处为0
		);
		glVertexArrayAttribBinding(		// 将顶点属性绑定到顶点数组对象的绑定点
			buffer.vao,					// vaobj：顶点数组对象的标识符
			i,							// attribindex：顶点属性索引
			i							// bindingindex：绑定点索引，通常与顶点属性索引相同
		);
	}
	return buffer;
}

void Buffer::DeleteMeshBuffer(MeshBuffer& buffer)
{
	if (buffer.vao) glDeleteVertexArrays(1, &buffer.vao);
	if (buffer.vbo) glDeleteBuffers(1, &buffer.vbo);
	if (buffer.ibo) glDeleteBuffers(1, &buffer.ibo);
	std::memset(&buffer, 0, sizeof(MeshBuffer));
}

FrameBuffer Buffer::CreateFrameBuffer(int width, int height, int samples, GLenum colorFormat, GLenum depthstencilFormat)
{
	FrameBuffer fb;
	fb.width = width;
	fb.height = height;
	fb.samples = samples;

	glCreateFramebuffers(1, &fb.id);

	if (colorFormat != GL_NONE) 
	{
		if (samples > 0) 
		{
			glCreateRenderbuffers(1, &fb.colorTarget);
			glNamedRenderbufferStorageMultisample(fb.colorTarget, samples, colorFormat, width, height);
			glNamedFramebufferRenderbuffer(fb.id, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fb.colorTarget);
		}
		else 
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &fb.colorTarget);
			glTextureStorage2D(fb.colorTarget, 1, colorFormat, width, height);
			glNamedFramebufferTexture(fb.id, GL_COLOR_ATTACHMENT0, fb.colorTarget, 0);
		}
	}
	if (depthstencilFormat != GL_NONE) 
	{
		glCreateRenderbuffers(1, &fb.depthStencilTarget);
		if (samples > 0) 
		{
			glNamedRenderbufferStorageMultisample(fb.depthStencilTarget, samples, depthstencilFormat, width, height);
		}
		else 
		{
			glNamedRenderbufferStorage(fb.depthStencilTarget, depthstencilFormat, width, height);
		}
		glNamedFramebufferRenderbuffer(fb.id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb.depthStencilTarget);
	}

	GLenum status = glCheckNamedFramebufferStatus(fb.id, GL_DRAW_FRAMEBUFFER);
	LOG_ASSERT(status != GL_FRAMEBUFFER_COMPLETE, "Framebuffer completeness check failed:" + std::to_string(status));
	return fb;
}

void Buffer::ResolveFramebuffer(const FrameBuffer& srcFB, const FrameBuffer& dstFB)
{
	if (srcFB.id == dstFB.id) return;

	std::vector<GLenum> attachments;
	if (srcFB.colorTarget) 
	{
		attachments.push_back(GL_COLOR_ATTACHMENT0);
	}
	if (srcFB.depthStencilTarget) 
	{
		attachments.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
	}
	assert(attachments.size() > 0);

	glBlitNamedFramebuffer(srcFB.id, dstFB.id, 0, 0, srcFB.width, srcFB.height, 0, 0, dstFB.width, dstFB.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glInvalidateNamedFramebufferData(srcFB.id, (GLsizei)attachments.size(), &attachments[0]);
}

void Buffer::DeleteFrameBuffer(FrameBuffer& fb)
{
	if (fb.id) 
	{
		glDeleteFramebuffers(1, &fb.id);
	}
	if (fb.colorTarget) 
	{
		if (fb.samples == 0) 
		{
			glDeleteTextures(1, &fb.colorTarget);
		}
		else 
		{
			glDeleteRenderbuffers(1, &fb.colorTarget);
		}
	}
	if (fb.depthStencilTarget) 
	{
		glDeleteRenderbuffers(1, &fb.depthStencilTarget);
	}
	std::memset(&fb, 0, sizeof(FrameBuffer));
}

GLuint Buffer::CreateUniformBuffer(const void* data, size_t size)
{
	GLuint ubo;						// Uniform Buffer Object（统一缓冲对象）
	glCreateBuffers(1, &ubo);
	glNamedBufferStorage(			// 为缓冲对象分配存储空间
		ubo,						// buffer：缓冲对象的标识符
		size,						// size：缓冲区的大小
		data,						// data：初始化缓冲区的数据指针
		GL_DYNAMIC_STORAGE_BIT		// flags：标志位，表示允许动态修改缓冲内容
	);
	return ubo;
}
