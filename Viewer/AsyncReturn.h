#pragma once
class AsyncReturn
{
public:
	virtual int sendAsyncReturn(int message, void *buf) = 0;
};

