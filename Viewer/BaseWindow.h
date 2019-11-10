#pragma once
class BaseWindow
{
public:
	BaseWindow() : m_hWnd(NULL), m_hAccel(NULL) { }

	HWND window() const { return m_hWnd; }
	virtual int NotifyAsyncReturn(int message) = 0;

	BOOL Create(
		PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hWndParent = 0,
		HMENU hMenu = 0,
		HACCEL hAccel = 0
	)
	{
		WNDCLASS wc = { 0 };
		wc.lpfnWndProc = BaseWindow::WndProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = ClassName();
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);

		RegisterClass(&wc);

		m_hWnd = CreateWindowEx(
			dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
			nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
		);
		m_hAccel = hAccel;

		return (m_hWnd ? TRUE : FALSE);

	}
	int TranslateAccel(LPMSG msg) {
		if (m_hAccel) {
			return ::TranslateAccelerator(m_hWnd, m_hAccel, msg);
		}
		return 0;
	}


private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		BaseWindow* pThis = NULL;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pThis = (BaseWindow*)pCreate->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

			pThis->m_hWnd = hWnd;
		}
		else {
			pThis = (BaseWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		}

		if (pThis) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else {
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}


protected:
	HWND m_hWnd;
	HACCEL m_hAccel;

	virtual PCWSTR ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

