#pragma once
#include <vector>
#include <memory>

class AsyncReturn;

class ImageStore
{
public:
	ImageStore();
	~ImageStore();

	HRESULT Initialize();

private:
	PCWSTR ClassName() const { return L"ImageStore"; }

	void CmdOpenFile(AsyncReturn*view, const WCHAR* file);
	void CmdNextFile(AsyncReturn* view);
	void CmdPrevFile(AsyncReturn* view);

	int FetchFiles(const WCHAR* path);
	int AppendFiles(const WCHAR* path, const WCHAR* filetype);

private:
	std::vector< std::unique_ptr<WCHAR[]> > m_files;
	int m_file_index;

	std::unique_ptr<BYTE[]> m_current_buf;
	std::unique_ptr<BYTE[]> m_next_buf;
	std::unique_ptr<BYTE[]> m_prev_buf;
};

