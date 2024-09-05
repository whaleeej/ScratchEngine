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
	device.Present(false);

	//for (int i = 0; i < kBackBufferCount; ++i)
	//{
	//	ID3D12Resource* resource = nullptr;
	//	s_SwapChain->GetBuffer(i, IID_PPV_ARGS(&resource));

	//	const FLOAT color[4] = { (i == 0 ? 1 : 0), (i == 1 ? 1 : 0), (i == 2 ? 1 : 0), 1 };

	//	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	//	rtvDesc.Format = swapChainDesc.Format;
	//	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//	rtvDesc.Texture2D.MipSlice = 0;
	//	rtvDesc.Texture2D.PlaneSlice = 0;

	//	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(cpuHandle, i, s_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	//	s_Device->CreateRenderTargetView(resource, &rtvDesc, handle);
	//	commandList->ClearRenderTargetView(cpuHandle, color, 0, nullptr);
	//}

	//commandList->Close();

	//ID3D12CommandList * const cmd = commandList;
	//s_Queue->ExecuteCommandLists(1, &cmd);
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
