#pragma once

class FileBuffer
{
public:
	FileBuffer() : m_bufsize(0) { }
	~FileBuffer() { }

	HRESULT Initialize(const WCHAR* file);
	const BYTE* getBuffer(size_t* size);

private:
	std::unique_ptr<BYTE[]> m_buf;
	size_t m_bufsize;
	std::mutex m_mutex;
};

