#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

#include "WindowApplication.h"
#include "D3D11On12.h"

int __stdcall WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ INT hCmdShow)
{
	D3D11On12 dxSample(1280, 720, "D3D11On12@orangelie");
	return WindowApplication::Run(&dxSample, hInstance, hCmdShow);
}