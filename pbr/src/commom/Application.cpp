#include <stdexcept>
#include <GLFW/glfw3.h>
#include "Application.h"
#include "Renderer.h"
#include "Path.h"


void Application::Init()
{
	mRenderer = new Renderer();
	glfwWindowHint(GLFW_RESIZABLE, 0);
	mWindow = mRenderer->Init();

	glfwSetWindowUserPointer(mWindow, this);
	glfwSetCursorPosCallback(mWindow, Application::MousePositionCallback);
	glfwSetMouseButtonCallback(mWindow, Application::MouseButtonCallback);
	glfwSetScrollCallback(mWindow, Application::MouseScrollCallback);
	glfwSetKeyCallback(mWindow, Application::KeyCallback);
}

void Application::Load()
{
	mRenderer->Load();
}

void Application::Run()
{
	mIsRun = !glfwWindowShouldClose(mWindow);
	mRenderer->RenderFrame(mWindow, mViewSettings, mSceneSettings);
	glfwPollEvents();
}

void Application::Clear()
{
	mRenderer->Clear();
	//glfwTerminate();
}

Application::Application()
	: mWindow(nullptr)
	, mPrevCursorX(0.0)
	, mPrevCursorY(0.0)
	, mInputMode(InputMode::None)
{
	LOG_ASSERT(!glfwInit(), "Failed to initialize GLFW library");

	mViewSettings.distance = gViewDistance;
	mViewSettings.fov      = gViewFOV;

	mSceneSettings.lights[0].direction = glm::normalize(glm::vec3{-1.0f,  0.0f, 0.0f});
	mSceneSettings.lights[1].direction = glm::normalize(glm::vec3{ 1.0f,  0.0f, 0.0f});
	mSceneSettings.lights[2].direction = glm::normalize(glm::vec3{ 0.0f, -1.0f, 0.0f});

	mSceneSettings.lights[0].radiance = glm::vec3{1.0f};
	mSceneSettings.lights[1].radiance = glm::vec3{1.0f};
	mSceneSettings.lights[2].radiance = glm::vec3{1.0f};
}

Application::~Application()
{
	if(mWindow)  glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void Application::MousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	Application* self = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	if(self->mInputMode != InputMode::None) {
		const double dx = xpos - self->mPrevCursorX;
		const double dy = ypos - self->mPrevCursorY;

		switch(self->mInputMode) {
		case InputMode::RotatingScene:
			self->mSceneSettings.yaw   += gOrbitSpeed * float(dx);
			self->mSceneSettings.pitch += gOrbitSpeed * float(dy);
			break;
		case InputMode::RotatingView:
			self->mViewSettings.yaw   += gOrbitSpeed * float(dx);
			self->mViewSettings.pitch += gOrbitSpeed * float(dy);
			break;
		}

		self->mPrevCursorX = xpos;
		self->mPrevCursorY = ypos;
	}
}
	
void Application::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Application* self = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

	const InputMode oldMode = self->mInputMode;
	if(action == GLFW_PRESS && self->mInputMode == InputMode::None) 
	{
		switch(button) {
		case GLFW_MOUSE_BUTTON_1:
			self->mInputMode = InputMode::RotatingView;
			break;
		case GLFW_MOUSE_BUTTON_2:
			self->mInputMode = InputMode::RotatingScene;
			break;
		}
	}
	if(action == GLFW_RELEASE && (button == GLFW_MOUSE_BUTTON_1 
		|| button == GLFW_MOUSE_BUTTON_2)) 
	{
		self->mInputMode = InputMode::None;
	}

	if(oldMode != self->mInputMode) 
	{
		if(self->mInputMode == InputMode::None) 
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else 
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwGetCursorPos(window, &self->mPrevCursorX, &self->mPrevCursorY);
		}
	}
}
	
void Application::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Application* self = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	self->mViewSettings.distance += gZoomSpeed * float(-yoffset);
}
	
void Application::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* self = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

	if(action == GLFW_PRESS) 
	{
		SceneSettings::Light* light = nullptr;
		
		switch(key) {
		case GLFW_KEY_F1:
			light = &self->mSceneSettings.lights[0];
			break;
		case GLFW_KEY_F2:
			light = &self->mSceneSettings.lights[1];
			break;
		case GLFW_KEY_F3:
			light = &self->mSceneSettings.lights[2];
			break;
		}

		if(light) 
		{
			light->enabled = !light->enabled;
		}
	}
}
