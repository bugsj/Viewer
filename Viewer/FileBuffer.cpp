#include "framework.h"

#include "FileBuffer.h"

HRESULT FileBuffer::Initialize(const WCHAR* filename)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	HRESULT hr = S_OK;

	if (m_bufsize != 0) {
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
		std::unique_ptr<BYTE[]> buf = std::make_unique<BYTE[]>(filesize.LowPart);
		DWORD rsize;
		if (!ReadFile(hfile, buf.get(), filesize.LowPart, &rsize, NULL)) {
			hr = E_FAIL;
		}
		else {
			m_buf = std::move(buf);
			m_bufsize = filesize.LowPart;
		}
	}
	CloseHandle(hfile);
	return hr;
}

const BYTE* FileBuffer::getBuffer(size_t *size)
{
	if (size == nullptr) {
		return nullptr;
	}

	if (m_bufsize != 0) {
		*size = m_bufsize;
		return m_buf.get();
	}
	else {
		*size = 0;
		return nullptr;
	}
	
}
