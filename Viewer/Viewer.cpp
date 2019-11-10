// Viewer.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ViewerApp.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR pszCmdLine,
	_In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pszCmdLine);

	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		{
			ViewerApp app;
			hr = app.Initialize(hInstance);

			if (SUCCEEDED(hr))
			{
				app.ShowWindow(nCmdShow);
				app.MessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}
