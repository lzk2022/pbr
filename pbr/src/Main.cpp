#include <cstdio>
#include <string>
#include <memory>
#include <vector>
#include "commom/Application.h"
#include "commom/Log.h"

int main()
{
	auto app = new Application();
	try
	{
		app->Init();
		app->Load();
		while (app->mIsRun)
		{
			app->Run();
		}
		app->Clear();
	}
	catch (const std::exception& e)
	{
		LOG_EXCEPTION(e.what());
		getchar();
	}
	
	return 0;
}
