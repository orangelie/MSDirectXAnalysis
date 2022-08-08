cmake_minimum_required (VERSION 3.8)

set(DEPENDENCIES "Dependencies/d3dx12.h")
set(D3D11On12 "D3D11On12.cpp" "D3D11On12.h")
set(WINAPPLICATION "WindowApplication.cpp" "WindowApplication.h")
set(DXSAMPLE "DXSample.cpp" "DXSample.h")
set(UTILS "Utils/Utils.cpp" "Utils/Utils.h" "D3D11On12.cpp" "D3D11On12.h" "Dependencies/d3dx12.h" "DXSample.cpp" "DXSample.h" "Main.cpp" "Utils/DXSampleHelper.cpp" "Utils/DXSampleHelper.h" "Utils/Utils.cpp" "Utils/Utils.h" "WindowApplication.cpp" "WindowApplication.h")
set(DXSAMHELPER "Utils/DXSampleHelper.cpp" "Utils/DXSampleHelper.h")

add_executable (ExecuteIndirect "Main.cpp" ${DEPENDENCIES} ${D3D11On12} ${WINAPPLICATION} ${DXSAMPLE} ${UTILS} ${DXSAMHELPER})
include_directories ("Dependencies/")