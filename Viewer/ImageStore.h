#pragma once
#include <vector>
#include <memory>
#include "FileBuffer.h"

class AsyncReturn;

class ImageStore
{
public:
	ImageStore();
	~ImageStore();

	HRESULT Initialize();

	void CmdOpenFile(AsyncReturn* view, const WCHAR* file);
	void CmdNextFile(AsyncReturn* view);
	void CmdPrevFile(AsyncReturn* view);

private:
	PCWSTR ClassName() const { return L"ImageStore"; }

	int FetchFiles(const WCHAR* path);
	int AppendFiles(const WCHAR* path, const WCHAR* filetype);
	int PointFile(const WCHAR* path);
	PCWSTR GetNextFilename(int step = 1) const;
	PCWSTR GetPrevFilename(int step = 1) const;

private:
	std::vector< std::unique_ptr<WCHAR[]> > m_files;
	int m_file_index;

	std::shared_ptr<FileBuffer> m_current_file;
	std::shared_ptr<FileBuffer> m_next_file;
	std::shared_ptr<FileBuffer> m_prev_file;
	std::shared_ptr<FileBuffer> m_null_file;
};

