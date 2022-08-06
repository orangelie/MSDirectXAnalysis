#pragma once

#include "Utils/DXSampleHelper.h"

class DXSample
{
public:
	DXSample() {}
	DXSample(INT clientWidth, INT clientHeight, const UNISTR clientName);

protected:
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnDraw() = 0;
	virtual void OnDestroy() = 0;

	const UNISTR mClientName = nullptr;
	INT mClientWidth, mClientHeight;

private:

};