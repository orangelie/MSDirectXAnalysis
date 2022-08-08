#include "WindowApplication.h"

HWND WindowApplication::gHwnd = nullptr;

int WindowApplication::Run(DXSample* dxSample, HINSTANCE hInstance, int hCmdShow)
{
	WNDCLASSEX wndClass = {};
	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wndClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = hInstance;
	wndClass.lpfnWndProc = WindowProcedure;
	wndClass.lpszClassName = "orangelieApp";
	wndClass.lpszMenuName = 0;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClassEx(&wndClass);

	RECT rect = { 0, 0, static_cast<LONG>(dxSample->GetWidth()), static_cast<LONG>(dxSample->GetHeight()) };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	gHwnd = CreateWindow(wndClass.lpszClassName, dxSample->GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top,
		nullptr, nullptr, hInstance, dxSample);

	dxSample->OnInit();
	ShowWindow(gHwnd, hCmdShow); /*> @ Warning @>> It must be in front of it.. <*/

	MSG msg = {};

	for (;;)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	dxSample->OnDestroy();

	return static_cast<char>(msg.wParam);
}

LRESULT __stdcall WindowApplication::WindowProcedure(HWND hWnd, UINT hMessage, WPARAM wParam, LPARAM lParam)
{
	DXSample* dxSample = reinterpret_cast<DXSample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (hMessage)
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT lpCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(lpCreateStruct->lpCreateParams));
		}
		return 0;
	case WM_PAINT:
		if (dxSample)
		{
			dxSample->OnUpdate();
			dxSample->OnDraw();
		}
		return 0;
	case WM_KEYUP:
		if (dxSample)
		{
			dxSample->OnKeyUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		return 0;
	case WM_KEYDOWN:
		if (dxSample)
		{
			dxSample->OnKeyDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		return 0;
	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO mmInfo = reinterpret_cast<LPMINMAXINFO>(lParam);
			mmInfo->ptMinTrackSize.x = 400;
			mmInfo->ptMinTrackSize.y = 300;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, hMessage, wParam, lParam);
}