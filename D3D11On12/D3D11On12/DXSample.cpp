#include "DXSample.h"

DXSample::DXSample(INT clientWidth, INT clientHeight, const UNISTR clientName, bool isWarpDevice)
{
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;
	mClientName = clientName;
	mIsWarpDevice = isWarpDevice;
}

const UNICHAR* DXSample::GetTitle() const
{
	return mClientName.c_str();
}

const INT DXSample::GetWidth() const
{
	return mClientWidth;
}

const INT DXSample::GetHeight() const
{
	return mClientHeight;
}