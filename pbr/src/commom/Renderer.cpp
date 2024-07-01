#include <iostream>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "Image.h"
#include "Utils.h"
#include "Renderer.h"
#include "Buffer.h"
#include "Shader.h"
#include "Log.h"
#include "Path.h"
#include <glm/gtc/type_ptr.hpp>

struct TransformUB
{
	glm::mat4 viewProjectionMatrix;
	glm::mat4 skyProjectionMatrix;
	glm::mat4 sceneRotationMatrix;
};

struct ShadingUB
{
	struct {
		glm::vec4 direction;
		glm::vec4 radiance;
	} lights[SceneSettings::NumLights];
	glm::vec4 eyePosition;
};


GLFWwindow* Renderer::Init()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DEPTH_BITS, 0);
	glfwWindowHint(GLFW_STENCIL_BITS, 0);
	glfwWindowHint(GLFW_SAMPLES, 0);

	GLFWwindow* window = glfwCreateWindow(gDisplaySizeX, gDisplaySizeY, "Physically Based Rendering (OpenGL 4.5)", nullptr, nullptr);
	LOG_ASSERT(!window, "Failed to create OpenGL context");

	glfwMakeContextCurrent(window);
	glfwSwapInterval(-1);

	bool ret = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	LOG_ASSERT(!ret, "Failed to initialize OpenGL extensions loader");

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gMaxAnisotropy);

#if _DEBUG
	glDebugMessageCallback(Renderer::LogMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	GLint maxSupportedSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSupportedSamples);

	const int samples = glm::min(gDisplaySamples, maxSupportedSamples);
	mFreameBuffer = Buffer::CreateFrameBuffer(gDisplaySizeX, gDisplaySizeY, samples, GL_RGBA16F, GL_DEPTH24_STENCIL8);
	if (samples > 0) 
	{
		mResolveFramebuffer = Buffer::CreateFrameBuffer(gDisplaySizeX, gDisplaySizeY, 0, GL_RGBA16F, GL_NONE);
	}
	else {
		mResolveFramebuffer = mFreameBuffer;
	}

	//LOG_INFO("OpenGL 4.5 Renderer"+ glGetString(GL_RENDERER));
	std::printf("OpenGL 4.5 Renderer [%s]\n", glGetString(GL_RENDERER));
	return window;
}

void Renderer::Load()
{
	//glEnable(GL_CULL_FACE);						// 启用面剔除
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);		// 启用无缝立方体贴图
	glFrontFace(GL_CCW);						// 设置正面为逆时针方向

	glCreateVertexArrays(1, &mEmptyVAO);		// 创建一个空的VAO用于渲染全屏三角形

	// 创建统一缓冲区
	mTransformUB = Buffer::CreateUniformBuffer<TransformUB>();
	mShadingUB = Buffer::CreateUniformBuffer<ShadingUB>();

	mTonemapProgram = Shader::LinkProgram({ "tonemap.vert","tonemap.frag" });

	mSkybox = Buffer::CreateMeshBuffer(Mesh::ReadFile("meshes/skybox.obj"));
	mSkyboxProgram = Shader::LinkProgram({ "skybox.vert","skybox.frag" });

	mPbrModel = Buffer::CreateMeshBuffer(Mesh::ReadFile("meshes/pbr.fbx"));
	mPbrProgram = Shader::LinkProgram({ "pbr.vert","pbr.frag" });

	mAlbedoTexture = Texture("textures/pbrA.png", 3, GL_RGB, GL_SRGB8);
	mNormalTexture = Texture("textures/pbrN.png", 3, GL_RGB, GL_RGB8);
	mMetalnessTexture = Texture("textures/pbrM.png", 1, GL_RED, GL_R8);
	mRoughnessTexture = Texture("textures/pbrR.png", 1, GL_RED, GL_R8);

	Texture envTextureUnfiltered = LoadAndConvertEquirectangularToCubemap();
	mEnvTexture = ComputePreFilteredSpecularMap(envTextureUnfiltered, gEnvMapSize);
	glDeleteTextures(1, &envTextureUnfiltered.mId);
	mIrmapTexture = ComputeDiffuseIrradianceCubemap(mEnvTexture, gIrradianceMapSize);
	mSpBRDF_LUT = ComputeCookTorranceBRDF_LUT(gBRDF_LUT_Size);

	glFinish();
}

void Renderer::Clear()
{
	if(mFreameBuffer.id != mResolveFramebuffer.id) 
	{
		Buffer::DeleteFrameBuffer(mResolveFramebuffer);
	}
	Buffer::DeleteFrameBuffer(mFreameBuffer);

	glDeleteVertexArrays(1, &mEmptyVAO);

	glDeleteBuffers(1, &mTransformUB);
	glDeleteBuffers(1, &mShadingUB);

	Buffer::DeleteMeshBuffer(mSkybox);
	Buffer::DeleteMeshBuffer(mPbrModel);
	
	glDeleteProgram(mTonemapProgram);
	glDeleteProgram(mSkyboxProgram);
	glDeleteProgram(mPbrProgram);

	mEnvTexture.DelTexture();
	mIrmapTexture.DelTexture();
	mSpBRDF_LUT.DelTexture();
	mAlbedoTexture.DelTexture();
	mNormalTexture.DelTexture();
	mMetalnessTexture.DelTexture();
	mRoughnessTexture.DelTexture();
}


void Renderer::RenderFrame(GLFWwindow* window, const ViewSettings& view, const SceneSettings& scene)
{

	// 创建一个简单的模型矩阵，并缩小为原来的一半
	glm::mat4 model = glm::mat4(1.0f); // 初始化为单位矩阵，即无变换
	model = glm::scale(model, glm::vec3(0.2f)); // 将模型缩小为原来的一半
	
	glUseProgram(mPbrProgram);
	GLuint modelLoc = glGetUniformLocation(mPbrProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	const glm::mat4 projectionMatrix = glm::perspectiveFov(view.fov, float(mFreameBuffer.width), float(mFreameBuffer.height), 1.0f, 1000.0f);
	const glm::mat4 viewRotationMatrix = glm::eulerAngleXY(glm::radians(view.pitch), glm::radians(view.yaw));
	const glm::mat4 sceneRotationMatrix = glm::eulerAngleXY(glm::radians(scene.pitch), glm::radians(scene.yaw));
	const glm::mat4 viewMatrix = glm::translate(glm::mat4{ 1.0f }, { 0.0f, 0.0f, -view.distance }) * viewRotationMatrix;
	const glm::vec3 eyePosition = glm::inverse(viewMatrix)[3];

	// 更新转换统一缓冲区
	{
		TransformUB transformUniforms;
		transformUniforms.viewProjectionMatrix = projectionMatrix * viewMatrix;
		transformUniforms.skyProjectionMatrix  = projectionMatrix * viewRotationMatrix;
		transformUniforms.sceneRotationMatrix  = sceneRotationMatrix;
		glNamedBufferSubData(mTransformUB, 0, sizeof(TransformUB), &transformUniforms);
	}

	// 更新着色统一缓冲区
	{
		ShadingUB shadingUniforms;
		shadingUniforms.eyePosition = glm::vec4(eyePosition, 0.0f);
		for(int i=0; i<SceneSettings::NumLights; ++i) 
		{
			const SceneSettings::Light& light = scene.lights[i];
			shadingUniforms.lights[i].direction = glm::vec4{light.direction, 0.0f};
			if(light.enabled) 
				shadingUniforms.lights[i].radiance = glm::vec4{light.radiance, 0.0f};
			else 
				shadingUniforms.lights[i].radiance = glm::vec4{};
		}
		glNamedBufferSubData(mShadingUB, 0, sizeof(ShadingUB), &shadingUniforms);
	}

	// 准备用于渲染的帧缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, mFreameBuffer.id);
	// 无需清除颜色，因为我们将用天空盒覆盖屏幕。
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// 绑定统一缓冲区
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, mTransformUB);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, mShadingUB);


	// 绘制天空盒
	glDisable(GL_DEPTH_TEST);
	glUseProgram(mSkyboxProgram);
	glBindTextureUnit(0, mEnvTexture.mId);
	glBindVertexArray(mSkybox.vao);
	glDrawElements(GL_TRIANGLES, mSkybox.numElements, GL_UNSIGNED_INT, 0);

	// 绘制 PBR 模型
	glEnable(GL_DEPTH_TEST);
	glUseProgram(mPbrProgram);
	/***********************satert 1*********************/
	glBindTextureUnit(0, mAlbedoTexture.mId);
	glBindTextureUnit(1, mNormalTexture.mId);
	glBindTextureUnit(2, mMetalnessTexture.mId);
	glBindTextureUnit(3, mRoughnessTexture.mId);
	/***********************end 1**************************/
	glBindTextureUnit(4, mEnvTexture.mId);
	glBindTextureUnit(5, mIrmapTexture.mId);
	glBindTextureUnit(6, mSpBRDF_LUT.mId);
	glBindVertexArray(mPbrModel.vao);
	glDrawElements(GL_TRIANGLES, mPbrModel.numElements, GL_UNSIGNED_INT, 0);
		
	// 解析多采样帧缓冲区
	Buffer::ResolveFramebuffer(mFreameBuffer, mResolveFramebuffer);

	// 绘制一个全屏三角形，用于后期处理/色调映射
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(mTonemapProgram);
	glBindTextureUnit(0, mResolveFramebuffer.colorTarget);
	glBindVertexArray(mEmptyVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glfwSwapBuffers(window);
}



Texture Renderer::LoadAndConvertEquirectangularToCubemap() {

	// 创建一个立方体贴图纹理，大小为 gEnvMapSize x gEnvMapSize，格式为 GL_RGBA16F。
	Texture envTextureUnfiltered = Texture(GL_TEXTURE_CUBE_MAP, gEnvMapSize, gEnvMapSize, GL_RGBA16F);
	// 链接并编译着色器程序，用于将等矩形贴图转换为立方体贴图。
	GLuint equirectToCubeProgram = Shader::LinkProgram({ "equirect2cube.comp" });
	Texture envTextureEquirect = Texture("environment.hdr", 3, GL_RGB, GL_RGB16F, 1);
	glUseProgram(equirectToCubeProgram);
	glBindTextureUnit(0, envTextureEquirect.mId);
	glBindImageTexture(				// 绑定未过滤的环境立方体贴图到图像单元0，用于写操作
		0,							// unit: 图像单元的索引
		envTextureUnfiltered.mId,	// texture: 要绑定的纹理对象的名称
		0,							// level: 要绑定的纹理的级别（mipmap 级别）
		GL_TRUE,					// layered: 是否绑定纹理图层
		0,							// layer: 要绑定的纹理图层的索引（如果 layered 为 GL_FALSE）
		GL_WRITE_ONLY,				// access: 指定如何访问图像的格式
		GL_RGBA16F					// format: 指定图像格式
	);

	// 启动计算着色器进行等矩形到立方体贴图的转换。
	// 计算着色器会被分配的工作组数为 (envTextureUnfiltered.mWidth / 32) x (envTextureUnfiltered.mHeight / 32) x 6。
	// 这里每个工作组处理 32x32 的像素块，共有 6 个立方体面。
	glDispatchCompute(envTextureUnfiltered.mWidth / 32, envTextureUnfiltered.mHeight / 32, 6);
	glDeleteTextures(1, &envTextureEquirect.mId);
	glDeleteProgram(equirectToCubeProgram);
	glGenerateTextureMipmap(envTextureUnfiltered.mId);		// 生成未过滤的环境立方体贴图的mipmap。
	return envTextureUnfiltered;
}
Texture Renderer::ComputePreFilteredSpecularMap(Texture& envUnfilteredT, int gEnvMapSize)
{
	// 链接并编译着色器程序，用于计算预过滤的镜面环境贴图。
	GLuint spmapProgram = Shader::LinkProgram({ "spmap.comp" });
	// 创建一个立方体贴图纹理，大小为 gEnvMapSize x gEnvMapSize，格式为 GL_RGBA16F。
	Texture mEnvTexture = Texture(GL_TEXTURE_CUBE_MAP, gEnvMapSize, gEnvMapSize, GL_RGBA16F);

	// 将第0级mipmap级别复制到目标环境贴图中。
	glCopyImageSubData(
		envUnfilteredT.mId, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, // 源纹理、类型、级别、x、y、z
		mEnvTexture.mId, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,    // 目标纹理、类型、级别、x、y、z
		mEnvTexture.mWidth, mEnvTexture.mHeight, 6           // 宽度、高度、深度（6个面）
	);

	// 使用预过滤镜面环境贴图的着色器程序。
	glUseProgram(spmapProgram);

	// 绑定未过滤的环境立方体贴图到纹理单元0。
	glBindTextureUnit(0, envUnfilteredT.mId);

	// 预过滤剩余的mipmap链。
	const float deltaRoughness = 1.0f / glm::max(float(mEnvTexture.mLevel - 1), 1.0f);
	for (int level = 1, size = gEnvMapSize / 2; level <= mEnvTexture.mLevel; ++level, size /= 2) {
		const GLuint numGroups = glm::max(1, size / 32);

		// 绑定目标环境贴图的指定级别到图像单元0，用于写操作，格式为 GL_RGBA16F。
		glBindImageTexture(
			0,                   // unit: 图像单元的索引
			mEnvTexture.mId,     // texture: 要绑定的纹理对象的名称
			level,               // level: 要绑定的纹理的级别（mipmap 级别）
			GL_TRUE,             // layered: 是否绑定纹理图层
			0,                   // layer: 要绑定的纹理图层的索引（如果 layered 为 GL_FALSE）
			GL_WRITE_ONLY,       // access: 指定如何访问图像的格式
			GL_RGBA16F           // format: 指定图像格式
		);

		// 将当前mipmap级别的粗糙度传递给着色器程序。
		glProgramUniform1f(spmapProgram, 0, level * deltaRoughness);

		// 启动计算着色器进行预过滤的镜面环境贴图计算。
		// 计算着色器会被分配的工作组数为 (numGroups x numGroups x 6)。
		// 这里每个工作组处理 32x32 的像素块，共有 6 个立方体面。
		glDispatchCompute(numGroups, numGroups, 6);
	}

	// 删除着色器程序，以释放资源。
	glDeleteProgram(spmapProgram);
	return mEnvTexture;
}
Texture Renderer::ComputeDiffuseIrradianceCubemap(const Texture& mEnvTexture, int gIrradianceMapSize)
{
	// 链接并编译着色器程序，用于计算漫反射辐照度立方体贴图。
	GLuint irmapProgram = Shader::LinkProgram({ "irmap.comp" });
	// 创建一个立方体贴图纹理，大小为 gIrradianceMapSize x gIrradianceMapSize，格式为 GL_RGBA16F。
	Texture mIrmapTexture = Texture(GL_TEXTURE_CUBE_MAP, gIrradianceMapSize, gIrradianceMapSize, GL_RGBA16F, 1);

	// 使用计算漫反射辐照度的着色器程序。
	glUseProgram(irmapProgram);

	// 绑定环境立方体贴图到纹理单元0。
	glBindTextureUnit(0, mEnvTexture.mId);

	// 绑定辐照度立方体贴图到图像单元0，用于写操作，格式为 GL_RGBA16F。
	glBindImageTexture(
		0,                     // unit: 图像单元的索引
		mIrmapTexture.mId,     // texture: 要绑定的纹理对象的名称
		0,                     // level: 要绑定的纹理的级别（mipmap 级别）
		GL_TRUE,               // layered: 是否绑定纹理图层
		0,                     // layer: 要绑定的纹理图层的索引（如果 layered 为 GL_FALSE）
		GL_WRITE_ONLY,         // access: 指定如何访问图像的格式
		GL_RGBA16F             // format: 指定图像格式
	);

	// 启动计算着色器进行漫反射辐照度立方体贴图的计算。
	// 计算着色器会被分配的工作组数为 (mIrmapTexture.mWidth / 32) x (mIrmapTexture.mHeight / 32) x 6。
	// 这里每个工作组处理 32x32 的像素块，共有 6 个立方体面。
	glDispatchCompute(mIrmapTexture.mWidth / 32, mIrmapTexture.mHeight / 32, 6);

	// 删除着色器程序，以释放资源。
	glDeleteProgram(irmapProgram);

	// 返回生成的漫反射辐照度立方体贴图。
	return mIrmapTexture;
}
Texture Renderer::ComputeCookTorranceBRDF_LUT(int gBRDF_LUT_Size) 
{
	// 链接并编译着色器程序，用于计算 Cook-Torrance BRDF 2D LUT。
	GLuint spBRDFProgram = Shader::LinkProgram({ "spbrdf.comp" });

	// 创建一个2D纹理，大小为 gBRDF_LUT_Size x gBRDF_LUT_Size，格式为 GL_RG16F。
	Texture mSpBRDF_LUT = Texture(GL_TEXTURE_2D, gBRDF_LUT_Size, gBRDF_LUT_Size, GL_RG16F, 1);

	// 设置纹理参数，使用边缘夹紧模式。
	glTextureParameteri(mSpBRDF_LUT.mId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(mSpBRDF_LUT.mId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 使用计算 Cook-Torrance BRDF 的着色器程序。
	glUseProgram(spBRDFProgram);

	// 绑定 BRDF LUT 纹理到图像单元0，用于写操作，格式为 GL_RG16F。
	glBindImageTexture(
		0,                   // unit: 图像单元的索引
		mSpBRDF_LUT.mId,     // texture: 要绑定的纹理对象的名称
		0,                   // level: 要绑定的纹理的级别（mipmap 级别）
		GL_FALSE,            // layered: 是否绑定纹理图层
		0,                   // layer: 要绑定的纹理图层的索引（如果 layered 为 GL_FALSE）
		GL_WRITE_ONLY,       // access: 指定如何访问图像的格式
		GL_RG16F             // format: 指定图像格式
	);

	// 启动计算着色器进行 Cook-Torrance BRDF 2D LUT 的计算。
	// 计算着色器会被分配的工作组数为 (mSpBRDF_LUT.mWidth / 32) x (mSpBRDF_LUT.mHeight / 32) x 1。
	// 这里每个工作组处理 32x32 的像素块。
	glDispatchCompute(mSpBRDF_LUT.mWidth / 32, mSpBRDF_LUT.mHeight / 32, 1);

	// 删除着色器程序，以释放资源。
	glDeleteProgram(spBRDFProgram);

	// 返回生成的 Cook-Torrance BRDF 2D LUT 纹理。
	return mSpBRDF_LUT;
}

#if _DEBUG

void Renderer::LogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	/*std::string msg = message;
	LOG_ASSERT(severity != GL_DEBUG_SEVERITY_NOTIFICATION,"GL: "+ msg);*/
	if(severity != GL_DEBUG_SEVERITY_NOTIFICATION) 
	{
		std::fprintf(stderr, "GL: %s\n", message);
	}
}
#endif


