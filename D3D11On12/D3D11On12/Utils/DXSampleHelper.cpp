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