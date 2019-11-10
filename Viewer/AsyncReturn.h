#pragma once

class AsyncReturn
{
public:
	virtual int sendAsyncReturn(HRESULT message, std::shared_ptr<FileBuffer> file) = 0;
};

