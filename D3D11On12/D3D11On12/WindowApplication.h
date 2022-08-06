#pragma once

#include "DXSample.h"

class DXSample;

class WindowApplication
{
public:
	WindowApplication() {}
	static int Run(DXSample* dxSample, HINSTANCE hInstance, int hCmdShow);
	static LRESULT __stdcall WindowProcedure(HWND hWnd, UINT hMessage, WPARAM wParam, LPARAM lParam);
	static HWND gHwnd;

private:

};