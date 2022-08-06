#pragma once

#include "Utils.h"

#ifdef UNICODE
#define UNISTR wchar_t*
#else
#define UNISTR char*
#endif // !UNICODE

std::string HrToString(HRESULT hResult);

class DxException : public std::runtime_error
{
public:
	DxException(HRESULT hResult) : std::runtime_error(HrToString(hResult)) { mResult = hResult; }
	HRESULT Error() const { return mResult; };

private:
	HRESULT mResult;

};