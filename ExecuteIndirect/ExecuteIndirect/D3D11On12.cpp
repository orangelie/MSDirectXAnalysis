#include "D3D11On12.h"

D3D11On12::D3D11On12(INT clientWidth, INT clientHeight, const UNISTR clientName, bool isWarpDevice) : DXSample(clientWidth, clientHeight, clientName, isWarpDevice)
{
}

void D3D11On12::OnInit()
{
	LoadGraphicsDevice();

	HR(mCommandList->Reset(mCommandAllocator.Get(), nullptr));
	LoadPipeline();
	HR(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();
}

void D3D11On12::OnUpdate()
{

}

void D3D11On12::OnDraw()
{
	PIXBeginEvent(mCommandQueue.Get(), 0, L"Render 3D");
	PopulateCommandList();
	PIXEndEvent(mCommandQueue.Get());

	PIXBeginEvent(mCommandQueue.Get(), 0, L"Render UI");
	RenderUI();
	PIXEndEvent(mCommandQueue.Get());

	HR(mSwapChain->Present(1, 0));
	mCurrBackbufferIndex = mSwapChain->GetCurrentBackBufferIndex(); // <= mCurrBackbufferIndex = (mCurrBackbufferIndex + 1) % 3;
	FlushCommandQueue();
}

void D3D11On12::OnKeyUp(WPARAM btnState, UINT x, UINT y)
{

}

void D3D11On12::OnKeyDown(WPARAM btnState, UINT x, UINT y)
{

}

void D3D11On12::OnDestroy()
{
	if (m12Device != nullptr)
	{
		FlushCommandQueue();
	}
}

void D3D11On12::LoadGraphicsDevice()
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

	/* Removing INVALID_HANDLE_VALUE Warning Statements from Debugging Layer. */
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

	/* DXSampleHelper.h > Remove warning statements that appear when "throw" in the debugging layer. */
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);

	/* D3D12 ERROR: ID3D12CommandQueue::Present: Resource state (0x800: D3D12_RESOURCE_STATE_COPY_SOURCE) (promoted from COMMON state) of resource (0x000001F6BE05D070:'Unnamed ID3D12Resource Object') (subresource: 0) must be in COMMON state when transitioning to use in a different Command List type, because resource state on previous Command List type : D3D12_COMMAND_LIST_TYPE_COPY, is actually incompatible and different from that on the next Command List type : D3D12_COMMAND_LIST_TYPE_DIRECT. [ RESOURCE_MANIPULATION ERROR #990: RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE]
D3D12: **BREAK** enabled for the previous message, which was: [ ERROR RESOURCE_MANIPULATION #990: RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE ]. */
	// This is a bug in the DXGI Debug Layer interaction with the DX12 Debug Layer Windows 11.
	// SOLUTION> https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
	D3D12_MESSAGE_ID hide[] =
	{
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
		// Workarounds for debug layer issues on hybrid-graphics systems
		D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
	};
	D3D12_INFO_QUEUE_FILTER filter = {};
	filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
	filter.DenyList.pIDList = hide;
	infoQueue->AddStorageFilterEntries(&filter);

	infoQueue->Release();
#endif

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = 0;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HR(m12Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(mCommandQueue.GetAddressOf())));
	HR(m12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())));
	HR(m12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())));
	HR(mCommandList->Close());

	HR(m12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())));

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

	mCurrBackbufferIndex = mSwapChain->GetCurrentBackBufferIndex();

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

	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
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

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < gBackBufferCount; ++i)
	{
		HR(mSwapChain->GetBuffer(i, IID_PPV_ARGS(mBackBufferPointer[i].GetAddressOf())));
		m12Device->CreateRenderTargetView(mBackBufferPointer[i].Get(), nullptr, rtvHandle);

		D3D11_RESOURCE_FLAGS resourceFlags = { D3D11_BIND_RENDER_TARGET };
		/*
		HR(m11On12Device->CreateWrappedResource(
			mBackBufferPointer[i].Get(), &resourceFlags,
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
			IID_PPV_ARGS(mWrappedBackbuffer[i].GetAddressOf())));
		*/
		HR(m11On12Device->CreateWrappedResource(
			mBackBufferPointer[i].Get(), &resourceFlags,
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT,
			IID_PPV_ARGS(mWrappedBackbuffer[i].GetAddressOf())));

		Microsoft::WRL::ComPtr<IDXGISurface> surface;
		HR(mWrappedBackbuffer[i].As(&surface));
		HR(mD2D1DeviceContext->CreateBitmapFromDxgiSurface(surface.Get(), &bitmapProperties, mD2D1Backbuffer[i].GetAddressOf()));

		rtvHandle.Offset(1, mRtvDescriptorHeapSize);
	}
}

void D3D11On12::LoadPipeline()
{
	{
		HR(mD2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Aqua), mSolidColorBrush.GetAddressOf()));
		HR(mDWriteFactory->CreateTextFormat(L"Verdana", nullptr,
			DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL,
			25, L"en-us", mDWriteTextFormat.GetAddressOf()));

		mDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		mDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
}

void D3D11On12::PopulateCommandList()
{
	HR(mCommandAllocator->Reset());
	HR(mCommandList->Reset(mCommandAllocator.Get(), nullptr));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackbufferIndex, mRtvDescriptorHeapSize);
	FLOAT backGroundColor[4] = { 0.75f, 0.25f, 0.25f, 1.0f };


	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		mBackBufferPointer[mCurrBackbufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	mCommandList->ClearRenderTargetView(rtvHandle, backGroundColor, 0, nullptr);

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		mBackBufferPointer[mCurrBackbufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	HR(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}

void D3D11On12::RenderUI()
{
	/*
		D2D DEBUG WARNING - EndDraw was called, but the rendertarget was not in a valid state. This may result from calling EndDraw without a matching BeginDraw.
		D2D DEBUG WARNING - A Flush call by a render target failed [88990001]. Tags [0x0, 0x0].
		D3D11 WARNING: ID3D11DeviceContext::OMSetRenderTargets: Resource being set to OM RenderTarget slot 0 is inaccessible because of a previous call to ReleaseSync or GetDC. [ STATE_SETTING WARNING #9: DEVICE_OMSETRENDERTARGETS_HAZARD]
		D2D DEBUG ERROR - An attempt to draw to an inaccessible target has been detected.
	*/

	D2D1_SIZE_F rtSize = mD2D1Backbuffer[mCurrBackbufferIndex]->GetSize();
	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height);
	static const WCHAR text[] = L"헤헿 D3D11On12 프로젝트 입니다.";

	m11On12Device->AcquireWrappedResources(mWrappedBackbuffer[mCurrBackbufferIndex].GetAddressOf(), 1);
	
	mD2D1DeviceContext->SetTarget(mD2D1Backbuffer[mCurrBackbufferIndex].Get());
	mD2D1DeviceContext->BeginDraw();
	mD2D1DeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	mD2D1DeviceContext->DrawTextA(text, _countof(text) - 1, mDWriteTextFormat.Get(), &textRect, mSolidColorBrush.Get());
	mD2D1DeviceContext->EndDraw();

	m11On12Device->ReleaseWrappedResources(mWrappedBackbuffer[mCurrBackbufferIndex].GetAddressOf(), 1);
	mImmediateContext->Flush();
}

void D3D11On12::FlushCommandQueue()
{
	++mCurrFenceCount;
	HR(mCommandQueue->Signal(mFence.Get(), mCurrFenceCount));

	if (mFence->GetCompletedValue() < mCurrFenceCount)
	{
		HANDLE hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		HR(mFence->SetEventOnCompletion(mCurrFenceCount, hEvent));

		WaitForSingleObjectEx(hEvent, 0xffffffff, FALSE);
		CloseHandle(hEvent);
	}
}
