#pragma once
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "d3d11")

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <wrl.h>
#include <windowsx.h>

#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include "d3dx12.h"

#include <d2d1.h>
#include <d2d1_3.h>
#include <dwrite.h>

#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <memory>
