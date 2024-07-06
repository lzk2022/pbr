#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <string>
#include <glad/glad.h>
#include "Buffer.h"
#include "RendererInterface.h"
#include "Texture.h"

class Renderer final : public RendererInterface
{
public:
	GLFWwindow* Init() override;
	void Load() override;

public:
	void Clear() override;
	void RenderFrame(GLFWwindow* window, const ViewSettings& view, const SceneSettings& scene) override;

private:

	 /********************************************************************************
	 * @brief		加载并将等矩形环境贴图转换为立方体贴图纹理。
	 *********************************************************************************
	 * @return      立方体贴图纹理
	 ********************************************************************************/
	Texture LoadAndConvertEquirectangularToCubemap();

	 /********************************************************************************
	 * @brief		计算预过滤的镜面环境贴图
	 *********************************************************************************
	 * @param		envUnfilteredT 输入的未过滤环境立方体贴图
	 * @param		gEnvMapSize 环境贴图的大小
	 * @return		输出的预过滤镜面环境立方体贴图
	 ********************************************************************************/
	Texture ComputePreFilteredSpecularMap(Texture& envUnfilteredT, int gEnvMapSize);

	 /********************************************************************************
	 * @brief		计算漫反射辐照度立方体贴图
	 *********************************************************************************
	 * @param       mEnvTexture 输入的环境立方体贴图
	 * @param		gIrradianceMapSize 辐照度贴图的大小
	 * @return		生成的漫反射辐照度立方体贴图
	 ********************************************************************************/
	Texture ComputeDiffuseIrradianceCubemap(const Texture& mEnvTexture, int gIrradianceMapSize);

	 /********************************************************************************
	 * @brief		计算用于分离和合并近似的 Cook-Torrance BRDF 2D LUT
	 *********************************************************************************
	 * @param		gBRDF_LUT_Size BRDF LUT 的大小
	 * @return		生成的 Cook-Torrance BRDF 2D LUT 纹理
	 ********************************************************************************/
	Texture ComputeCookTorranceBRDF_LUT(int gBRDF_LUT_Size);

#if _DEBUG
	static void LogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif


	FrameBuffer mFreameBuffer;			// 帧缓冲对象
	FrameBuffer mResolveFramebuffer;	// 解析帧缓冲对象
	MeshBuffer mSkybox;					// 天空盒网格缓冲
	MeshBuffer mPbrModel;				// PBR模型网格缓冲
	GLuint mEmptyVAO;					// 空的顶点数组对象
	GLuint mTonemapProgram;				// 色调映射程序
	GLuint mSkyboxProgram;				// 天空盒程序
	GLuint mPbrProgram;					// PBR程序

	Texture mEnvTexture;				// 环境贴图纹理
	Texture mIrmapTexture;				// 辐照度贴图纹理
	Texture mSpBRDF_LUT;				// 镜面BRDF查找表纹理
	Texture mAlbedoTexture;				// 反照率纹理
	Texture mNormalTexture;				// 法线纹理
	Texture mMetalnessTexture;			// 金属度纹理
	Texture mRoughnessTexture;			// 粗糙度纹理

	GLuint mTransformUB;				// 变换统一缓冲对象
	GLuint mShadingUB;					// 光照统一缓冲对象

	bool mIsSrc = true;

};

#endif // !__RENDERER_H__