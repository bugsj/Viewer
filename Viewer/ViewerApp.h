#pragma once

#include "ViewWindow.h"

class ViewerApp
{
public:
	ViewerApp();
	~ViewerApp();

	HRESULT Initialize(HINSTANCE hInstance);
	BOOL ShowWindow(int nCmdShow);
	void MessageLoop();

private:

private:
	HINSTANCE			m_hInst;
	IWICImagingFactory* m_pIWICFactory;
	ID2D1Factory*		m_pD2DFactory;

	ViewWindow			m_ViewWindow;
};

