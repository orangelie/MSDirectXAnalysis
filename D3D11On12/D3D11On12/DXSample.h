#pragma once

#include "Utils/DXSampleHelper.h"
#include "WindowApplication.h"

class DXSample
{
public:
	DXSample() {}
	DXSample(INT clientWidth, INT clientHeight, const UNISTR clientName, bool isWarpDevice);
	const UNICHAR* GetTitle() const;
	const INT GetWidth() const;
	const INT GetHeight() const;

	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDraw() = 0;
	virtual void OnKeyUp(WPARAM btnState, UINT x, UINT y) = 0;
	virtual void OnKeyDown(WPARAM btnState, UINT x, UINT y) = 0;
	virtual void OnDestroy() = 0;

protected:
	UNISTR mClientName;
	INT mClientWidth, mClientHeight;
	bool mIsWarpDevice;

private:

};