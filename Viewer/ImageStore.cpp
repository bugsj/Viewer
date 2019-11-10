#include "framework.h"

#include <algorithm>
#include <PathCch.h>

#include "ImageStore.h"
#include "BaseWindow.h"


ImageStore::ImageStore() : m_file_index(0)
{}

ImageStore::~ImageStore()
{}

HRESULT ImageStore::Initialize()
{
	return S_OK;
}

int ImageStore::AppendFiles(const WCHAR* path, const WCHAR* filetype)
{
	size_t lenpath = wcslen(path);
	size_t lentype = wcslen(filetype);
	std::unique_ptr<WCHAR[]> extpath = std::make_unique<WCHAR[]>(lenpath + lentype + 2);
	CopyMemory(extpath.get(), path, sizeof(WCHAR) * lenpath);
	extpath[lenpath] = L'\\';
	CopyMemory(extpath.get() + lenpath + 1, filetype, sizeof(WCHAR) * (lentype + 1));

	m_files.clear();

	WIN32_FIND_DATA nextinfo;
	HANDLE file = FindFirstFile(extpath.get(), &nextinfo);
	if (file == INVALID_HANDLE_VALUE) {
		return 0;
	}
	size_t filenamelen = wcslen(nextinfo.cFileName);
	m_files.emplace_back(std::make_unique<WCHAR[]>(lenpath + filenamelen + 2));
	CopyMemory(m_files.back().get(), path, sizeof(WCHAR) * lenpath);
	m_files.back()[lenpath] = L'\\';
	CopyMemory(m_files.back().get() + lenpath + 1, nextinfo.cFileName, sizeof(WCHAR) * (filenamelen + 1));

	while (FindNextFile(file, &nextinfo)) {
		filenamelen = wcslen(nextinfo.cFileName);
		m_files.emplace_back(std::make_unique<WCHAR[]>(lenpath + filenamelen + 2));
		CopyMemory(m_files.back().get(), path, sizeof(WCHAR) * lenpath);
		m_files.back()[lenpath] = L'\\';
		CopyMemory(m_files.back().get() + lenpath + 1, nextinfo.cFileName, sizeof(WCHAR) * (filenamelen + 1));
	}

	return static_cast<int>(m_files.size());
}
int ImageStore::FetchFiles(const WCHAR* path)
{
	m_files.clear();
	AppendFiles(path, L"*.jpg");
	AppendFiles(path, L"*.png");

	std::sort(m_files.begin(), m_files.end(), [](const std::unique_ptr<WCHAR[]>& l, const std::unique_ptr<WCHAR[]>& r) {
		return wcscmp(l.get(), r.get()) < 0;
		});

	return static_cast<int>(m_files.size());
}

void ImageStore::CmdOpenFile(BaseWindow* view, const WCHAR* filename)
{
	size_t strsize = wcslen(filename) + 1;
	std::unique_ptr<WCHAR[]> path = std::make_unique<WCHAR[]>(strsize);
	CopyMemory(path.get(), filename, strsize);
	HRESULT hr = PathCchRemoveFileSpec(path.get(), strsize);
	if (hr == S_OK) {
		FetchFiles(path.get());
	}

	if (filename)
	{
		HANDLE hfile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile == INVALID_HANDLE_VALUE) {
			view->NotifyAsyncReturn(-1);
		}
		LARGE_INTEGER filesize;
		if (!GetFileSizeEx(hfile, &filesize)) {
			view->NotifyAsyncReturn(-1);
		}
		std::unique_ptr<BYTE[]> buf = std::make_unique<BYTE[]>(filesize.QuadPart);
		DWORD rsize;
		if (!ReadFile(hfile, buf.get(), filesize.QuadPart, &rsize, NULL)) {
			view->NotifyAsyncReturn(-1);
		}
		m_current_buf = std::move(buf);
		CloseHandle(hfile);
	}
}

void ImageStore::CmdNextFile(BaseWindow* view)
{

}

void ImageStore::CmdPrevFile(BaseWindow* view)
{

}
