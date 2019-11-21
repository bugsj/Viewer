#pragma once

class FileBuffer
{
public:
	FileBuffer() { }
	~FileBuffer() { }

	HRESULT Initialize(const WCHAR* file);
	HRESULT Initialize(size_t size, BYTE** buf);

	const BYTE* getBuffer(size_t* size);
	const std::vector<BYTE>& getBuffer() { return m_buf; }

private:
	FileBuffer(const FileBuffer& rhs) = delete; //½ûÖ¹¸´ÖÆ
	FileBuffer& operator=(const FileBuffer& rhs) = delete; //½ûÖ¹¸´ÖÆ

private:
	std::vector<BYTE> m_buf;
	std::mutex m_mutex;
};

