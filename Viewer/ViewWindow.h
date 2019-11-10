#pragma once
#include "BaseWindow.h"
#include <vector>
#include <memory>

class ViewWindow : public BaseWindow
{
public:
	ViewWindow();
	~ViewWindow();

	HRESULT Initialize(HINSTANCE hInstance, IWICImagingFactory* pIWICFactory, ID2D1Factory* pD2DFactory);

private:
	HRESULT CreateD2DBitmapFromFile();
	HRESULT CreateD2DBitmapFromFile(LPCWSTR pszFileName);
	HRESULT CreateD2DBitmapFromMemory(BYTE *buf, size_t size);

	BOOL LocateImageFile(LPWSTR pszFileName, DWORD cbFileName);
	HRESULT CreateDeviceResources();

	PCWSTR ClassName() const { return L"D2DWICViewer"; }

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMenu(WORD identifier);
	LRESULT HandleAccel(WORD identifier);
	LRESULT OnPaint();

	void CmdOpenFile();
	void CmdClose() { PostMessage(m_hWnd, WM_CLOSE, 0, 0); }
	void CmdNextFile();
	void CmdPrevFile();

	int FetchFiles(const WCHAR *path);
	int AppendFiles(const WCHAR* path, const WCHAR *filetype);

private:

	IWICImagingFactory*		m_pIWICFactory;

	ID2D1Factory*			m_pD2DFactory;
	ID2D1HwndRenderTarget*	m_pRT;
	ID2D1Bitmap*			m_pD2DBitmap;
	IWICFormatConverter*	m_pConvertedSourceBitmap;

	std::vector< std::unique_ptr<WCHAR[]> > m_files;
	int m_file_index;
};

