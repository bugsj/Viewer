#include "pch.h"
#include "CppUnitTest.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#include "../Viewer/FileBuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ViewerUnitTest
{
	TEST_CLASS(FileBufferUnitTest)
	{
	public:
		
		TEST_METHOD(FileBufferInit)
		{
			FileBuffer fb1;
			Assert::AreEqual(fb1.Initialize(L""), E_FAIL);
			Assert::AreEqual(fb1.Initialize(L"d:\\SHARE\\pic\\02.jpg"), S_OK);
			Assert::AreEqual(fb1.Initialize(L"d:\\SHARE\\pic\\02.jpg"), E_FAIL);
			Assert::AreEqual(fb1.Initialize(L"d:\\SHARE\\pic\\024.jpg"), E_FAIL);

			FileBuffer fb2;
			Assert::AreEqual(fb2.Initialize(L"d:\\SHARE\\pic\\024.jpg"), S_OK);
			Assert::AreEqual(fb2.Initialize(L"d:\\SHARE\\pic\\02.jpg"), E_FAIL);
		}
		TEST_METHOD(FileBufferGet)
		{
			FileBuffer fb1;
			size_t size;
			const BYTE* buf;
			buf = fb1.getBuffer(&size);
			Assert::AreEqual(reinterpret_cast<int>(buf), 0);
			Assert::AreEqual(static_cast<int>(size), 0);
			buf = fb1.getBuffer(nullptr);
			Assert::AreEqual(reinterpret_cast<int>(buf), 0);
			Assert::AreEqual(fb1.Initialize(L"d:\\SHARE\\pic\\02.jpg"), S_OK);
			buf = fb1.getBuffer(&size);
			Assert::AreNotEqual(reinterpret_cast<int>(buf), 0);
			const int test_file_size = 339631;
			Assert::AreEqual(static_cast<int>(size), test_file_size);
			Assert::AreEqual(*reinterpret_cast<const int *>(buf), static_cast<int>(0xE0FFD8FF));
			Assert::AreEqual(static_cast<int>(buf[test_file_size - 1]), static_cast<int>(0xD9));
			buf = fb1.getBuffer(0);
			Assert::AreEqual(reinterpret_cast<int>(buf), 0);
		}
	};
}
