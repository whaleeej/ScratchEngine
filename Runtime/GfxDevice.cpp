#include "GfxDevice.h"
#include <d3dx12.h>

const int k_D3D12AdapterIndex = 0;
const int k_BackBufferCount = 3;

GfxDevice::GfxDevice()
	: m_DXGIFactory(nullptr)
	, m_DXGIAdapter(nullptr)
	, m_DXGISwapChain(nullptr)
	, m_Device(nullptr)
	, m_CommandQueue(nullptr)
	, m_CommandAllocator(nullptr)
	, m_GraphicsCommandList(nullptr)
	, m_DescriptorHeap(nullptr)
	, m_Fence(nullptr)
	, m_FenceValue(0)
	, m_FenceEvent()
{
}

GfxDevice::~GfxDevice()
{
	if (m_DXGIFactory != nullptr)
		m_DXGIFactory->Release();

	if (m_DXGIAdapter != nullptr)
		m_DXGIAdapter->Release();

	if (m_DXGISwapChain != nullptr)
		m_DXGISwapChain->Release();

	if (m_Device != nullptr)
		m_Device->Release();

	if (m_CommandQueue != nullptr)
		m_CommandQueue->Release();

	if (m_CommandAllocator != nullptr)
		m_CommandAllocator->Release();

	if (m_GraphicsCommandList != nullptr)
		m_GraphicsCommandList->Release();

	if (m_DescriptorHeap != nullptr)
		m_DescriptorHeap->Release();

	if (m_Fence != nullptr)
		m_Fence->Release();

	CloseHandle(m_FenceEvent);
}

static D3D12_COMMAND_QUEUE_DESC GetDirectCommandQueueDesc()
{
	D3D12_COMMAND_QUEUE_DESC desc;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = 0;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	return desc;
}

static DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc(UINT width, UINT height)
{
	DXGI_SWAP_CHAIN_DESC1 desc;
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = k_BackBufferCount;
	desc.Scaling = DXGI_SCALING_NONE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	return desc;
}

static D3D12_DESCRIPTOR_HEAP_DESC GetRTVDescriptorHeapDesc(UINT count)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.NumDescriptors = count;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;
	return desc;
}

bool GfxDevice::Initialize(HWND hWnd, UINT width, UINT height, bool debug)
{
	UINT createDXGIFactoryFlags = 0;

	ID3D12Debug* debugController;
	if (debug && SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->Release();

		createDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

	if (FAILED(CreateDXGIFactory2(createDXGIFactoryFlags, IID_PPV_ARGS(&m_DXGIFactory))))
	{
		return false;
	}

	IDXGIAdapter* adapter = nullptr;
	for (int i = 0; SUCCEEDED(m_DXGIFactory->EnumAdapters(i, &adapter)); ++i)
	{
		if (i == k_D3D12AdapterIndex)
			break;

		adapter->Release();
	}
	if (adapter == nullptr)
	{
		return false;
	}

	if (FAILED(adapter->QueryInterface(IID_PPV_ARGS(&m_DXGIAdapter))))
	{
		adapter->Release();
		return false;
	}
	adapter->Release();

	if (FAILED(D3D12CreateDevice(m_DXGIAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device))))
	{
		return false;
	}

	if (FAILED(m_Device->CreateCommandQueue(&GetDirectCommandQueueDesc(), IID_PPV_ARGS(&m_CommandQueue))))
	{
		return false;
	}

	if (FAILED(m_DXGIFactory->CreateSwapChainForHwnd(m_CommandQueue, hWnd, &GetSwapChainDesc(width, height), nullptr, NULL, &m_DXGISwapChain)))
	{
		return false;
	}

	if (FAILED(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator))))
	{
		return false;
	}

	if (FAILED(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, IID_PPV_ARGS(&m_GraphicsCommandList))))
	{
		return false;
	}

	if (FAILED(m_Device->CreateDescriptorHeap(&GetRTVDescriptorHeapDesc(k_BackBufferCount), IID_PPV_ARGS(&m_DescriptorHeap))))
	{
		return false;
	}

	if (FAILED(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence))))
	{
		return false;
	}

	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		return false;
	}

	return true;
}

void GfxDevice::Present(bool vSync)
{
	HRESULT hr = m_DXGISwapChain->Present((vSync ? 1 : 0), (vSync ? 0 : DXGI_PRESENT_ALLOW_TEARING));
	if (FAILED(hr))
	{
		printf("d3d12: swapchain present failed (%x).\n", hr);
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			HRESULT reason = m_Device->GetDeviceRemovedReason();
			printf("Device removed reason (%x).\n", reason);
		}
	}

	m_FenceValue++;

	m_CommandQueue->Signal(m_Fence, m_FenceValue);

	if (m_Fence->GetCompletedValue() < m_FenceValue)
	{
		m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

GfxDevice& GetGfxDevice()
{
	static GfxDevice* s_GfxDevice = nullptr;
	if (s_GfxDevice == nullptr)
	{
		s_GfxDevice = new GfxDevice();
	}
	return *s_GfxDevice;
}