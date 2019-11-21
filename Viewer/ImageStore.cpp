#include "framework.h"

#include <algorithm>
#include <PathCch.h>

#include "ImageStore.h"
#include "AsyncReturn.h"


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
	m_files.emplace_back(std::vector<WCHAR>(lenpath + filenamelen + 2));
	CopyMemory(m_files.back().data(), path, sizeof(WCHAR) * lenpath);
	m_files.back()[lenpath] = L'\\';
	CopyMemory(m_files.back().data() + lenpath + 1, nextinfo.cFileName, sizeof(WCHAR) * (filenamelen + 1));

	while (FindNextFile(file, &nextinfo)) {
		filenamelen = wcslen(nextinfo.cFileName);
		m_files.emplace_back(std::vector<WCHAR>(lenpath + filenamelen + 2));
		CopyMemory(m_files.back().data(), path, sizeof(WCHAR) * lenpath);
		m_files.back()[lenpath] = L'\\';
		CopyMemory(m_files.back().data() + lenpath + 1, nextinfo.cFileName, sizeof(WCHAR) * (filenamelen + 1));
	}

	return static_cast<int>(m_files.size());
}
int ImageStore::FetchFiles(const WCHAR* path)
{
	m_files.clear();
	AppendFiles(path, L"*.jpg");
	AppendFiles(path, L"*.png");

	std::sort(m_files.begin(), m_files.end(), [](const std::vector<WCHAR>& l, const std::vector<WCHAR>& r) {
		return wcscmp(l.data(), r.data()) < 0;
	});

	return static_cast<int>(m_files.size());
}

int ImageStore::PointFile(const WCHAR* file)
{
	if (m_files.empty()) {
		m_current_file = 0;
		return 0;
	}
	auto cur = std::find_if(m_files.begin(), m_files.end(), [file](std::vector<WCHAR>& p) {
		return wcscmp(p.data(), file) == 0;
	});
	m_file_index = (cur - m_files.begin()) % m_files.size();
	return m_file_index;
}

PCWSTR ImageStore::GetNextFilename(int step) const
{
	size_t file_cnt = m_files.size();
	switch (file_cnt) {
	case 0:
		return nullptr;
	case 1:
		return m_files[0].data();
	default:
		return m_files[(m_file_index + step) % file_cnt].data();
	}
}
PCWSTR ImageStore::GetPrevFilename(int step) const
{
	size_t file_cnt = m_files.size();
	switch (file_cnt) {
	case 0:
		return nullptr;
	case 1:
		return m_files[0].data();
	default:
		return m_files[(m_file_index - step) % file_cnt].data();
	}
}

void ImageStore::CmdOpenFile(AsyncReturn* view, const WCHAR* filename)
{
	size_t strsize = wcslen(filename) + 1;
	std::unique_ptr<WCHAR[]> path = std::make_unique<WCHAR[]>(strsize);
	CopyMemory(path.get(), filename, strsize);
	HRESULT hr = PathCchRemoveFileSpec(path.get(), strsize);
	int filelist_size = 0;
	if (hr == S_OK) {
		filelist_size = FetchFiles(path.get());
		PointFile(filename);
	}

	std::unique_ptr<FileBuffer> file = std::make_unique<FileBuffer>();
	if (file->Initialize(filename) == S_OK)
	{
		m_current_file = std::move(file);
		switch (filelist_size) {
		case 0:
		case 1:
			m_next_file = m_current_file;
			m_prev_file = m_current_file;
			break;
		case 2:
			if (file->Initialize(GetNextFilename()) == S_OK) {
				m_next_file = std::move(file);
				m_prev_file = m_next_file;
			}
			break;
		default:
			if (file->Initialize(GetNextFilename()) == S_OK) {
				m_next_file = std::move(file);
			}
			if (file->Initialize(GetPrevFilename()) == S_OK) {
				m_prev_file = std::move(file);
			}

		}
		view->sendAsyncReturn(S_OK, m_current_file);
	}
	else {
		view->sendAsyncReturn(E_FAIL, m_null_file);
	}
}

void ImageStore::CmdNextFile(AsyncReturn* view)
{
	size_t file_cnt = m_files.size();
	switch (file_cnt) {
	case 0:
		view->sendAsyncReturn(E_FAIL, m_null_file);
		break;
	case 1:
		view->sendAsyncReturn(E_FAIL, m_null_file);
		break;
	case 2:
		m_prev_file = m_current_file;
		m_current_file = m_next_file;
		m_next_file = m_prev_file;
		view->sendAsyncReturn(S_OK, m_current_file);
		break;
	case 3:
		m_current_file.swap(m_next_file);
		m_next_file.swap(m_prev_file);
		view->sendAsyncReturn(S_OK, m_current_file);
		break;
	default:
		std::unique_ptr<FileBuffer> next = std::make_unique<FileBuffer>();
		m_prev_file = m_current_file;
		m_current_file = m_next_file;
		if (next->Initialize(GetNextFilename()) == S_OK) {
			m_next_file = std::move(next);
		}
		view->sendAsyncReturn(S_OK, m_current_file);
		break;
	}
}

void ImageStore::CmdPrevFile(AsyncReturn* view)
{
	size_t file_cnt = m_files.size();
	switch (file_cnt) {
	case 0:
		view->sendAsyncReturn(E_FAIL, m_null_file);
		break;
	case 1:
		view->sendAsyncReturn(E_FAIL, m_null_file);
		break;
	case 2:
		m_prev_file = m_current_file;
		m_current_file = m_next_file;
		m_next_file = m_prev_file;
		view->sendAsyncReturn(S_OK, m_current_file);
		break;
	case 3:
		m_current_file.swap(m_prev_file);
		m_next_file.swap(m_prev_file);
		view->sendAsyncReturn(S_OK, m_current_file);
		break;
	default:
		std::unique_ptr<FileBuffer> prev = std::make_unique<FileBuffer>();
		m_next_file = m_current_file;
		m_current_file = m_prev_file;
		if (prev->Initialize(GetNextFilename()) == S_OK) {
			m_prev_file = std::move(prev);
		}
		view->sendAsyncReturn(S_OK, m_current_file);
		break;
	}
}
