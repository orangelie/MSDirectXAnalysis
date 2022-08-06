#include "DXSampleHelper.h"

std::string HrToString(HRESULT hResult)
{
	char result[0x50] = {};
#pragma warning(push)
#pragma warning(disable:4996)
	std::sprintf(result, "HRESULT of 0x%08X", static_cast<UINT>(hResult));
#pragma warning(pop)
	return std::string(result);
}

void GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool requestHighPerformanceAdapter)
{
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter1 = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
	HR(CreateDXGIFactory1(IID_PPV_ARGS(factory6.GetAddressOf())));

	for (UINT adapterIndex = 0;
		SUCCEEDED(factory6->EnumAdapterByGpuPreference(adapterIndex,
			requestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
			IID_PPV_ARGS(adapter1.GetAddressOf()))); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 adapter1Descriptor = {};
		HR(adapter1->GetDesc1(&adapter1Descriptor));

		if (adapter1Descriptor.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	if (adapter1 == nullptr)
	{
		for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapters1(adapterIndex, adapter1.GetAddressOf())); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 adapter1Descriptor = {};
			HR(adapter1->GetDesc1(&adapter1Descriptor));

			if (adapter1Descriptor.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			if (SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*adapter = adapter1.Detach();
}