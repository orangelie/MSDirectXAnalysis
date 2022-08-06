#pragma once

#include "DXSample.h"

class D3D11On12 : public DXSample
{
public:
	D3D11On12() {}
	D3D11On12(INT clientWidth, INT clientHeight, const UNISTR clientName);

protected:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnDraw();
	virtual void OnKeyUp(WPARAM btnState, UINT x, UINT y);
	virtual void OnKeyDown(WPARAM btnState, UINT x, UINT y);
	virtual void OnDestroy();

private:

private:


};