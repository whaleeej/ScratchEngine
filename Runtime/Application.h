#pragma once
#include "GfxDevice.h"

class Application
{
public:
	Application();
	~Application();

public:
	void Update();
	void Render();

private:
	UINT64 m_FrameCount;
	double m_Time;
};

Application& GetApplication();