#include "framework.h"

#include "FileBuffer.h"

HRESULT FileBuffer::Initialize(const WCHAR* filename)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	HRESULT hr = S_OK;

	if (m_buf.size() != 0) {
		return E_FAIL;
	}

	HANDLE hfile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		return E_FAIL;
	}
	LARGE_INTEGER filesize;
	if (!GetFileSizeEx(hfile, &filesize) || filesize.QuadPart == 0) {
		hr = E_FAIL;
	}
	else {
		Assert(filesize.HighPart == 0);
		std::vector<BYTE> buf(filesize.LowPart);
		DWORD rsize;
		if (!ReadFile(hfile, buf.data(), filesize.LowPart, &rsize, NULL)) {
			hr = E_FAIL;
		}
		else {
			Assert(rsize == filesize.LowPart);
			m_buf.swap(buf);
		}
	}
	CloseHandle(hfile);
	return hr;
}

HRESULT FileBuffer::Initialize(size_t size, BYTE** buf)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	HRESULT hr = S_OK;

	if (m_buf.size() != 0 || size == 0) {
		return E_FAIL;
	}
	
	m_buf.resize(size);
	*buf = m_buf.data();

	return hr;
}

const BYTE* FileBuffer::getBuffer(size_t *size)
{
	if (size == nullptr) {
		return nullptr;
	}

	if (m_buf.size() != 0) {
		*size = m_buf.size();
		return m_buf.data();
	}
	else {
		*size = 0;
		return nullptr;
	}
	
}
