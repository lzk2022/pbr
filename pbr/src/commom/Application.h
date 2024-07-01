#pragma once
#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <memory>
#include "RendererInterface.h"

class Application
{
public:
	Application();
	~Application();

public:
	void Init();
	void Load();
	void Run();
	void Clear();

private:
	static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
	RendererInterface* mRenderer;
	bool mIsRun = true;

private:
	GLFWwindow* mWindow;
	double mPrevCursorX;
	double mPrevCursorY;

	ViewSettings mViewSettings;
	SceneSettings mSceneSettings;

	enum class InputMode
	{
		None,
		RotatingView,
		RotatingScene,
	};
	InputMode mInputMode;
};

#endif // !__APPLICATION_H__