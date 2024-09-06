#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

class GfxDevice
{
public:
	GfxDevice();
	~GfxDevice();
	bool Initialize(HWND hWnd, UINT width, UINT height, UINT backBufferCount, bool debug);

public:
	void Begin();
	void ResetBackBuffers(FLOAT* color);
	void Submit();
	void Present(bool vSync);

private:
	IDXGIFactory2* m_DXGIFactory;
	IDXGIAdapter3* m_DXGIAdapter;
	IDXGISwapChain1* m_DXGISwapChain;

	ID3D12Device* m_Device;
	ID3D12CommandQueue* m_CommandQueue;
	ID3D12CommandAllocator* m_CommandAllocator;
	ID3D12GraphicsCommandList* m_GraphicsCommandList;
	ID3D12DescriptorHeap* m_DescriptorHeap;

	ID3D12Fence* m_Fence;
	UINT64 m_FenceValue;
	HANDLE m_FenceEvent;

	UINT m_BackBufferCount;
	UINT m_BackBufferIndex;
};

GfxDevice& GetGfxDevice();