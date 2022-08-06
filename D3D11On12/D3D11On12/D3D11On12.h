#pragma once

#include "DXSample.h"

class D3D11On12 : public DXSample
{
public:
	D3D11On12() {}
	D3D11On12(INT clientWidth, INT clientHeight, const UNISTR clientName, bool isWarpDevice);

protected:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnDraw();
	virtual void OnKeyUp(WPARAM btnState, UINT x, UINT y);
	virtual void OnKeyDown(WPARAM btnState, UINT x, UINT y);
	virtual void OnDestroy();

private:
	void LoadPipeline();

private:
	const static UINT gBackBufferCount = 3;
	UINT mBackBufferIndex = 0;
	UINT mRtvDescriptorHeapSize;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> m12Device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;

	Microsoft::WRL::ComPtr<ID3D11On12Device> m11On12Device = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mImmediateContext = nullptr;

	Microsoft::WRL::ComPtr<ID2D1Factory3> mD2D1Factory = nullptr;
	Microsoft::WRL::ComPtr<ID2D1Device2> mD2D1Device = nullptr;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext2> mD2D1DeviceContext = nullptr;
	Microsoft::WRL::ComPtr<IDWriteFactory> mDWriteFactory = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvDescriptorHeap = nullptr;
};