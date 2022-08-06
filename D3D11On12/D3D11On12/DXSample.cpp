#include "DXSample.h"

DXSample::DXSample(INT clientWidth, INT clientHeight, const UNISTR clientName)
{
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;
	mClientName = clientName;
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