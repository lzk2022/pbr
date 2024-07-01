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
	glNamedBufferStorage(										// Ϊ��������������洢�ռ�
		buffer.vbo,												// buffer.vbo������������ı�ʶ��
		vertexDataSize,											// size�����ݴ�С
		reinterpret_cast<const void*>(&mesh->mVertices[0]),		// data������ָ���ת��
		0														// flags����־λ���˴�δʹ��
	);

	glCreateBuffers(1, &buffer.ibo);
	glNamedBufferStorage(buffer.ibo, indexDataSize, reinterpret_cast<const void*>(&mesh->mTriangle[0]), 0);

	glCreateVertexArrays(1, &buffer.vao);
	// �������������󶨵�����������󣬴Ӷ������������ݵ���������
	glVertexArrayElementBuffer(buffer.vao, buffer.ibo);
	for (int i = 0; i < Mesh::mkNumAttributes; ++i) 
	{
		glVertexArrayVertexBuffer(		// �����㻺��󶨵������������
			buffer.vao,					// vaobj�������������ı�ʶ��
			i,							// bindingindex���󶨵�����
			buffer.vbo,					// buffer�����㻺�����ı�ʶ��
			i * sizeof(glm::vec3),		// offset��ƫ���������ֽ�Ϊ��λ
			sizeof(Mesh::Vertex)		// stride��������ָ��ÿ����������֮��ļ��
		);
		glEnableVertexArrayAttrib(		// ���ö����������Ķ�������
			buffer.vao,					// vaobj�������������ı�ʶ��
			i							// index��������������
		);
		GLint size = i == (Mesh::mkNumAttributes - 1) ? 2 : 3;
		glVertexArrayAttribFormat(		// ���ö����������Ķ������Ը�ʽ
			buffer.vao,					// vaobj�������������ı�ʶ��
			i,							// index��������������
			size,						// size��ÿ���������Ե��������
			GL_FLOAT,					// type���������ͣ��˴�Ϊ������
			GL_FALSE,					// normalized���Ƿ���Ҫ���ݹ�һ�����˴�Ϊ��
			0							// relativeoffset�����ƫ�������˴�Ϊ0
		);
		glVertexArrayAttribBinding(		// ���������԰󶨵������������İ󶨵�
			buffer.vao,					// vaobj�������������ı�ʶ��
			i,							// attribindex��������������
			i							// bindingindex���󶨵�������ͨ���붥������������ͬ
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
	GLuint ubo;						// Uniform Buffer Object��ͳһ�������
	glCreateBuffers(1, &ubo);
	glNamedBufferStorage(			// Ϊ����������洢�ռ�
		ubo,						// buffer���������ı�ʶ��
		size,						// size���������Ĵ�С
		data,						// data����ʼ��������������ָ��
		GL_DYNAMIC_STORAGE_BIT		// flags����־λ����ʾ����̬�޸Ļ�������
	);
	return ubo;
}
