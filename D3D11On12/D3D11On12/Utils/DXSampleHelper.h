#pragma once

#include "Utils.h"

#ifdef UNICODE
#define UNISTR	std::wstring
#define UNICHAR wchar_t
#else
#define UNISTR	std::string
#define UNICHAR char
#endif

std::string HrToString(HRESULT hResult);

class DxException : public std::runtime_error
{
public:
	DxException(HRESULT hResult) : std::runtime_error(HrToString(hResult)) { mResult = hResult; }
	HRESULT Error() const { return mResult; };

private:
	HRESULT mResult;

};