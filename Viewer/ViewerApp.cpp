#include "framework.h"
#include "ViewerApp.h"

ViewerApp::ViewerApp()
	:
	m_pIWICFactory(NULL),
	m_pD2DFactory(NULL),
	m_hInst(NULL)
{
}

/******************************************************************
*  Tear down resources                                            *
******************************************************************/

ViewerApp::~ViewerApp()
{
	SafeRelease(m_pIWICFactory);
	SafeRelease(m_pD2DFactory);
}

HRESULT ViewerApp::Initialize(HINSTANCE hInstance)
{
	m_hInst = hInstance;

	HRESULT hr = S_OK;

	// Create WIC factory
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pIWICFactory)
	);

	if (SUCCEEDED(hr))
	{
		// Create D2D factory
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_ViewWindow.Initialize(hInstance, m_pIWICFactory, m_pD2DFactory);
	}

	return hr;
}

BOOL ViewerApp::ShowWindow(int nCmdShow)
{
	return ::ShowWindow(m_ViewWindow.window(), nCmdShow);
}

void ViewerApp::MessageLoop()
{
	BOOL fRet;
	MSG msg;

	// Main message loop:
	while ((fRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (fRet == -1)
		{
			break;
		}
		else
		{
			if (!m_ViewWindow.TranslateAccel(&msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}
