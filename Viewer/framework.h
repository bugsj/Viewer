// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include <d2d1.h>
#include <d2d1helper.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

template <typename T>
inline void SafeRelease(T*& p)
{
	if (NULL != p)
	{
		p->Release();
		p = NULL;
	}
}

const FLOAT DEFAULT_DPI = 96.f;   // Default DPI that maps image resolution directly to screen resoltuion

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif
