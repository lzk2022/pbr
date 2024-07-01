#pragma once
#ifndef __RENDERERINTERFACE_H__
#define __RENDERERINTERFACE_H__

#include <glm/mat4x4.hpp>

struct GLFWwindow;

struct ViewSettings
{
	float pitch = 0.0f;
	float yaw = 0.0f;
	float distance;
	float fov;
};

struct SceneSettings
{
	float pitch = 0.0f;
	float yaw = 0.0f;

	static const int NumLights = 3;
	struct Light {
		glm::vec3 direction;
		glm::vec3 radiance;
		bool enabled = false;
	} lights[NumLights];
};

class RendererInterface
{
public:
	virtual GLFWwindow* Init() = 0;
	virtual void Load() = 0;

public:
	virtual ~RendererInterface() = default;

	virtual void Clear() = 0;
	virtual void RenderFrame(GLFWwindow* window, const ViewSettings& view, const SceneSettings& scene) = 0;
};

#endif // !__RENDERERINTERFACE_H__