#include "D3D11On12.h"

D3D11On12::D3D11On12(INT clientWidth, INT clientHeight, const UNISTR clientName, bool isWarpDevice) : DXSample(clientWidth, clientHeight, clientName, isWarpDevice)
{
}

void D3D11On12::OnInit()
{
	LoadPipeline();
}

void D3D11On12::OnUpdate()
{

}

void D3D11On12::OnDraw()
{

}

void D3D11On12::OnKeyUp(WPARAM btnState, UINT x, UINT y)
{

}

void D3D11On12::OnKeyDown(WPARAM btnState, UINT x, UINT y)
{

}

void D3D11On12::OnDestroy()
{

}

void D3D11On12::LoadPipeline()
{
	UINT dxgiFactoryFlag = 0;
	UINT d3d11DeviceFlag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS factoryOptions = {};

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> D3Ddebugger;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(D3Ddebugger.GetAddressOf()))))
	{
		D3Ddebugger->EnableDebugLayer();

		dxgiFactoryFlag |= DXGI_CREATE_FACTORY_DEBUG;
		factoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
		d3d11DeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
	HR(CreateDXGIFactory2(dxgiFactoryFlag, IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

	if (mIsWarpDevice)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
		HR(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf())));
		HR(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m12Device.GetAddressOf())));
	}
	else
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(dxgiFactory.Get(), hardwareAdapter.GetAddressOf(), true);
		HR(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(m12Device.GetAddressOf())));
	}

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
	HR(m12Device->QueryInterface(infoQueue.GetAddressOf()));

	D3D12_MESSAGE_SEVERITY messageSeverity[] = {
		D3D12_MESSAGE_SEVERITY_INFO
	};

	D3D12_MESSAGE_ID messageID[] = {
		D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE
	};

	D3D12_INFO_QUEUE_FILTER infoQueueFilter = {};
	infoQueueFilter.DenyList.pSeverityList = messageSeverity;
	infoQueueFilter.DenyList.NumSeverities = _countof(messageSeverity);
	infoQueueFilter.DenyList.pIDList = messageID;
	infoQueueFilter.DenyList.NumIDs = _countof(messageID);

	HR(infoQueue->PushStorageFilter(&infoQueueFilter));
#endif

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = 0;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HR(m12Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = {};
	swapChainDesc1.BufferCount = gBackBufferCount;
	swapChainDesc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc1.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc1.Width = mClientWidth;
	swapChainDesc1.Height = mClientHeight;
	swapChainDesc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc1.Scaling = DXGI_SCALING_NONE;
	swapChainDesc1.SampleDesc.Count = 1;
	swapChainDesc1.SampleDesc.Quality = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain = nullptr;
	HR(dxgiFactory->CreateSwapChainForHwnd(mCommandQueue.Get(),
		WindowApplication::gHwnd, &swapChainDesc1, nullptr, nullptr, swapChain.GetAddressOf()));

	HR(dxgiFactory->MakeWindowAssociation(WindowApplication::gHwnd, DXGI_MWA_NO_ALT_ENTER));
	HR(swapChain.As(&mSwapChain));

	mBackBufferIndex = mSwapChain->GetCurrentBackBufferIndex();

	Microsoft::WRL::ComPtr<ID3D11Device> device11 = nullptr;
	HR(D3D11On12CreateDevice(m12Device.Get(),
		d3d11DeviceFlag, nullptr, 0, reinterpret_cast<IUnknown**>(mCommandQueue.GetAddressOf()),
		1, 0, device11.GetAddressOf(), mImmediateContext.GetAddressOf(), nullptr));
	HR(device11.As(&m11On12Device));

	{
		D2D1_DEVICE_CONTEXT_OPTIONS deviceContextOption = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
		HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &factoryOptions, &mD2D1Factory));
		Microsoft::WRL::ComPtr<IDXGIDevice> device;
		HR(m11On12Device.As(&device));
		HR(mD2D1Factory->CreateDevice(device.Get(), mD2D1Device.GetAddressOf()));
		HR(mD2D1Device->CreateDeviceContext(deviceContextOption, &mD2D1DeviceContext));
		HR(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &mDWriteFactory));
	}

	FLOAT dpiX, dpiY;
#pragma warning(push)
#pragma warning(disable:4996)
	mD2D1Factory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)

	D2D1_BITMAP_PROPERTIES1 bitmapProperties =D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptor = {};
		rtvDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDescriptor.NodeMask = 0;
		rtvDescriptor.NumDescriptors = gBackBufferCount;
		rtvDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

		HR(m12Device->CreateDescriptorHeap(&rtvDescriptor, IID_PPV_ARGS(mRtvDescriptorHeap.GetAddressOf())));
		mRtvDescriptorHeapSize = m12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}


}
