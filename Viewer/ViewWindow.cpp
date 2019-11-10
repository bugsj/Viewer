#include "framework.h"

#include <algorithm>

#include <PathCch.h>

#include "ViewWindow.h"
#include "Resource.h"

ViewWindow::ViewWindow()
	:
	m_pD2DBitmap(NULL),
	m_pConvertedSourceBitmap(NULL),
	m_pIWICFactory(NULL),
	m_pD2DFactory(NULL),
	m_pRT(NULL),
	m_file_index(0)
{
}

/******************************************************************
*  Tear down resources                                            *
******************************************************************/

ViewWindow::~ViewWindow()
{
	SafeRelease(m_pD2DBitmap);
	SafeRelease(m_pConvertedSourceBitmap);
	SafeRelease(m_pRT);
}


HRESULT ViewWindow::Initialize(HINSTANCE hInstance, IWICImagingFactory* pIWICFactory, ID2D1Factory* pD2DFactory)
{
	m_pIWICFactory = pIWICFactory;
	m_pD2DFactory = pD2DFactory;

	return Create(
		L"WIC Viewer D2D Sample",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0,
		CW_USEDEFAULT, CW_USEDEFAULT,
		640, 480,
		0,
		LoadMenu(NULL, MAKEINTRESOURCE(IDR_MAINMENU)),
		LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_ACCEL))
	) ? S_OK : E_FAIL;
}

/******************************************************************
*  Load an image file and create an D2DBitmap                     *
******************************************************************/

HRESULT ViewWindow::CreateD2DBitmapFromFile()
{
	HRESULT hr = S_OK;

	WCHAR szFileName[MAX_PATH];

	// Step 1: Create the open dialog box and locate the image file
	if (LocateImageFile(szFileName, ARRAYSIZE(szFileName)))
	{
		hr = CreateD2DBitmapFromFile(szFileName);
	}

	return hr;
}

HRESULT ViewWindow::CreateD2DBitmapFromFile(LPCWSTR szFileName)
{
	HRESULT hr = S_OK;

	// Step 1: Create the open dialog box and locate the image file
	if (szFileName)
	{
		HANDLE file = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE) {
			return E_FAIL;
		}
		LARGE_INTEGER filesize;
		if (!GetFileSizeEx(file, &filesize)) {
			return E_FAIL;
		}

		Assert(filesize.HighPart == 0);
		std::unique_ptr<BYTE[]> buf(new BYTE[filesize.LowPart]);
		DWORD rsize;
		if (!ReadFile(file, buf.get(), filesize.LowPart, &rsize, NULL)) {
			return E_FAIL;
		}
		CloseHandle(file);
		hr = CreateD2DBitmapFromMemory(buf.get(), rsize);
	}

	return hr;
}
HRESULT ViewWindow::CreateD2DBitmapFromMemory(BYTE* buf, size_t size)
{
	HRESULT hr = S_OK;

	// Step 1: Create the open dialog box and locate the image file
	if (buf)
	{
		// Step 2: Decode the source image

		IWICStream* pStream;
		hr = m_pIWICFactory->CreateStream(&pStream);
		pStream->InitializeFromMemory(buf, size);

		// Create a decoder
		IWICBitmapDecoder* pDecoder = NULL;

		hr = m_pIWICFactory->CreateDecoderFromStream(
			pStream,                      // Image to be decoded
			NULL,                            // Do not prefer a particular vendor
			WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
			&pDecoder                        // Pointer to the decoder
		);

		// Retrieve the first frame of the image from the decoder
		IWICBitmapFrameDecode* pFrame = NULL;

		if (SUCCEEDED(hr))
		{
			hr = pDecoder->GetFrame(0, &pFrame);
		}

		//Step 3: Format convert the frame to 32bppPBGRA
		if (SUCCEEDED(hr))
		{
			SafeRelease(m_pConvertedSourceBitmap);
			hr = m_pIWICFactory->CreateFormatConverter(&m_pConvertedSourceBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pConvertedSourceBitmap->Initialize(
				pFrame,                          // Input bitmap to convert
				GUID_WICPixelFormat32bppPBGRA,   // Destination pixel format
				WICBitmapDitherTypeNone,         // Specified dither pattern
				NULL,                            // Specify a particular palette 
				0.f,                             // Alpha threshold
				WICBitmapPaletteTypeCustom       // Palette translation type
			);
		}

		//Step 4: Create render target and D2D bitmap from IWICBitmapSource
		if (SUCCEEDED(hr))
		{
			hr = CreateDeviceResources();
		}

		if (SUCCEEDED(hr))
		{
			// Need to release the previous D2DBitmap if there is one
			SafeRelease(m_pD2DBitmap);
			hr = m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
		}

		SafeRelease(pDecoder);
		SafeRelease(pFrame);
		SafeRelease(pStream);
	}

	return hr;
}
/******************************************************************
* Creates an open file dialog box and locate the image to decode. *
******************************************************************/

BOOL ViewWindow::LocateImageFile(LPWSTR pszFileName, DWORD cchFileName)
{
	pszFileName[0] = L'\0';

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFilter = L"All Image Files\0"              L"*.bmp;*.dib;*.wdp;*.mdp;*.hdp;*.gif;*.png;*.jpg;*.jpeg;*.tif;*.ico\0"
		L"Windows Bitmap\0"               L"*.bmp;*.dib\0"
		L"High Definition Photo\0"        L"*.wdp;*.mdp;*.hdp\0"
		L"Graphics Interchange Format\0"  L"*.gif\0"
		L"Portable Network Graphics\0"    L"*.png\0"
		L"JPEG File Interchange Format\0" L"*.jpg;*.jpeg\0"
		L"Tiff File\0"                    L"*.tif\0"
		L"Icon\0"                         L"*.ico\0"
		L"All Files\0"                    L"*.*\0"
		L"\0";
	ofn.lpstrFile = pszFileName;
	ofn.nMaxFile = cchFileName;
	ofn.lpstrTitle = L"Open Image";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	// Display the Open dialog box. 
	BOOL rs = GetOpenFileName(&ofn);
	if (rs) {
		size_t strsize = wcslen(ofn.lpstrFile) + 1;
		std::unique_ptr<WCHAR[]> path(new WCHAR[strsize]);
		CopyMemory(path.get(), ofn.lpstrFile, strsize);
		HRESULT hr = PathCchRemoveFileSpec(path.get(), strsize);
		if (hr == S_OK) {
			FetchFiles(path.get());
		}
	}
	return rs;
}

int ViewWindow::AppendFiles(const WCHAR* path, const WCHAR* filetype)
{
	size_t lenpath = wcslen(path);
	size_t lentype = wcslen(filetype);
	std::unique_ptr<WCHAR[]> jpgpath(new WCHAR[lenpath + lentype + 2]);
	CopyMemory(jpgpath.get(), path, sizeof(WCHAR) * lenpath);
	jpgpath[lenpath] = L'\\';
	CopyMemory(jpgpath.get() + lenpath + 1, filetype, sizeof(WCHAR) * (lentype + 1));

	m_files.clear();

	WIN32_FIND_DATA nextinfo;
	HANDLE file = FindFirstFile(jpgpath.get(), &nextinfo);
	if (file == INVALID_HANDLE_VALUE) {
		return 0;
	}
	size_t filenamelen = wcslen(nextinfo.cFileName);
	m_files.emplace_back(new WCHAR[lenpath + filenamelen + 2]);
	CopyMemory(m_files.back().get(), path, sizeof(WCHAR) * lenpath);
	m_files.back()[lenpath] = L'\\';
	CopyMemory(m_files.back().get() + lenpath + 1, nextinfo.cFileName, sizeof(WCHAR) * (filenamelen + 1));

	while (FindNextFile(file, &nextinfo)) {
		filenamelen = wcslen(nextinfo.cFileName);
		m_files.emplace_back(new WCHAR[lenpath + filenamelen + 2]);
		CopyMemory(m_files.back().get(), path, sizeof(WCHAR) * lenpath);
		m_files.back()[lenpath] = L'\\';
		CopyMemory(m_files.back().get() + lenpath + 1, nextinfo.cFileName, sizeof(WCHAR) * (filenamelen + 1));
	}

	return static_cast<int>(m_files.size());
}
int ViewWindow::FetchFiles(const WCHAR* path)
{
	m_files.clear();
	AppendFiles(path, L"*.jpg");
	AppendFiles(path, L"*.png");

	std::sort(m_files.begin(), m_files.end(), [](const std::unique_ptr<WCHAR[]>& l, const std::unique_ptr<WCHAR[]>& r) {
		return wcscmp(l.get(), r.get()) < 0;
		});

	return static_cast<int>(m_files.size());
}


/******************************************************************
*  This method creates resources which are bound to a particular  *
*  D2D device. It's all centralized here, in case the resources   *
*  need to be recreated in the event of D2D device loss           *
* (e.g. display change, remoting, removal of video card, etc).    *
******************************************************************/

HRESULT ViewWindow::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRT)
	{
		RECT rc;
		hr = GetClientRect(m_hWnd, &rc) ? S_OK : E_FAIL;

		if (SUCCEEDED(hr))
		{
			// Create a D2D render target properties
			D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties();

			// Set the DPI to be the default system DPI to allow direct mapping
			// between image pixels and desktop pixels in different system DPI settings
			renderTargetProperties.dpiX = DEFAULT_DPI;
			renderTargetProperties.dpiY = DEFAULT_DPI;

			// Create a D2D render target
			D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

			hr = m_pD2DFactory->CreateHwndRenderTarget(
				renderTargetProperties,
				D2D1::HwndRenderTargetProperties(m_hWnd, size),
				&m_pRT
			);
		}
	}

	return hr;
}


/******************************************************************
*  Internal Window message handler                                *
******************************************************************/

LRESULT ViewWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		// Parse the menu selections:
		if (HIWORD(wParam) == 0) {
			return HandleMenu(LOWORD(wParam));
		}
		else if (HIWORD(wParam) == 1) {
			return HandleAccel(LOWORD(wParam));
		}
		break;
	case WM_SIZE:
		D2D1_SIZE_U size = D2D1::SizeU(LOWORD(lParam), HIWORD(lParam));

		if (m_pRT)
		{
			// If we couldn't resize, release the device and we'll recreate it
			// during the next render pass.
			if (FAILED(m_pRT->Resize(size)))
			{
				SafeRelease(m_pRT);
				SafeRelease(m_pD2DBitmap);
			}
		}
		break;
	case WM_PAINT:
		return OnPaint();
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT ViewWindow::HandleMenu(WORD cmd_id)
{
	switch (cmd_id)
	{
	case IDM_FILE:
		CmdOpenFile();
		break;
	case IDM_EXIT:
		CmdClose();
		break;
	}
	return 0;
}

LRESULT ViewWindow::HandleAccel(WORD cmd_id)
{
	switch (cmd_id)
	{
	case IDM_NEXTIMAGE:
		CmdNextFile();
		break;
	case IDM_PREVIMAGE:
		CmdPrevFile();
		break;
	}
	return 0;
	return 0;
}

void ViewWindow::CmdOpenFile()
{
	if (SUCCEEDED(CreateD2DBitmapFromFile()))
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else
	{
		MessageBox(m_hWnd, L"Failed to load image, select a new one.", L"Application Error", MB_ICONEXCLAMATION | MB_OK);
	}
}

void ViewWindow::CmdPrevFile()
{
	if (m_files.size() <= 1) {
		return;
	}
	m_file_index = (m_file_index - 1) % m_files.size();
	if (SUCCEEDED(CreateD2DBitmapFromFile(m_files[m_file_index].get())))
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else
	{
		MessageBox(m_hWnd, L"Failed to load image, select a new one.", L"Application Error", MB_ICONEXCLAMATION | MB_OK);
	}
}

void ViewWindow::CmdNextFile()
{
	if (m_files.size() <= 1) {
		return;
	}
	m_file_index = (m_file_index + 1) % m_files.size();
	if (SUCCEEDED(CreateD2DBitmapFromFile(m_files[m_file_index].get())))
	{
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else
	{
		MessageBox(m_hWnd, L"Failed to load image, select a new one.", L"Application Error", MB_ICONEXCLAMATION | MB_OK);
	}
}

/******************************************************************
* Rendering routine using D2D                                     *
******************************************************************/
LRESULT ViewWindow::OnPaint()
{
	HRESULT hr = S_OK;
	PAINTSTRUCT ps;

	if (BeginPaint(m_hWnd, &ps))
	{
		// Create render target if not yet created
		hr = CreateDeviceResources();

		if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
		{
			m_pRT->BeginDraw();

			m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

			// Clear the background
			m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

			D2D1_SIZE_F rtSize = m_pRT->GetSize();

			// Create a rectangle same size of current window
			D2D1_RECT_F rectangle = D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height);

			// D2DBitmap may have been released due to device loss. 
			// If so, re-create it from the source bitmap
			if (m_pConvertedSourceBitmap && !m_pD2DBitmap)
			{
				m_pRT->CreateBitmapFromWicBitmap(m_pConvertedSourceBitmap, NULL, &m_pD2DBitmap);
			}

			// Draws an image and scales it to the current window size
			if (m_pD2DBitmap)
			{
				m_pRT->DrawBitmap(m_pD2DBitmap, rectangle);
			}

			hr = m_pRT->EndDraw();

			// In case of device loss, discard D2D render target and D2DBitmap
			// They will be re-create in the next rendering pass
			if (hr == D2DERR_RECREATE_TARGET)
			{
				SafeRelease(m_pD2DBitmap);
				SafeRelease(m_pRT);
				// Force a re-render
				hr = InvalidateRect(m_hWnd, NULL, TRUE) ? S_OK : E_FAIL;
			}
		}

		EndPaint(m_hWnd, &ps);
	}

	return SUCCEEDED(hr) ? 0 : 1;
}
