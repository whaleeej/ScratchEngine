#include "Application.h"
#include <stdio.h>
#include <chrono>

Application::Application()
	: m_FrameCount(0)
	, m_Time(0)
{
}

Application::~Application()
{
}

void Application::Update()
{
	m_FrameCount++;

	static std::chrono::steady_clock::time_point startTime = std::chrono::high_resolution_clock::now();
	m_Time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0;

	printf("Application::Update():%lld:%f\n", m_FrameCount, m_Time);
}

void Application::Render()
{
	GfxDevice& device = GetGfxDevice();
	device.Begin();
	device.ResetBackBuffers(nullptr);
	device.Submit();
	device.Present(true);
}

Application& GetApplication()
{
	static Application* s_Application = nullptr;
	if (s_Application == nullptr)
	{
		s_Application = new Application();
	}
	return *s_Application;
}
