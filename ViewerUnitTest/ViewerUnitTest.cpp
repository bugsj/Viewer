#include "pch.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CppUnitTest.h"
#include "crc32.h"

#include "../Viewer/FileBuffer.h"
#include "../Viewer/ImageStore.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ViewerUnitTest
{
	TEST_CLASS(FileBufferUnitTest)
	{
	public:
		
		TEST_METHOD(FileBufferInit)
		{
			FileBuffer fb1;
			Assert::AreEqual(E_FAIL, fb1.Initialize(L""));
			Assert::AreEqual(S_OK, fb1.Initialize(L"d:\\SHARE\\pic\\02.jpg"));
			Assert::AreEqual(E_FAIL, fb1.Initialize(L"d:\\SHARE\\pic\\02.jpg"));
			Assert::AreEqual(E_FAIL, fb1.Initialize(L"d:\\SHARE\\pic\\024.jpg"));

			FileBuffer fb2;
			Assert::AreEqual(S_OK, fb2.Initialize(L"d:\\SHARE\\pic\\Thumbs.db"));
			Assert::AreEqual(E_FAIL, fb2.Initialize(L"d:\\SHARE\\pic\\02.jpg"));
		}

		TEST_METHOD(FileBufferGet)
		{
			FileBuffer fb1;
			size_t size;
			const BYTE* buf;
			buf = fb1.getBuffer(&size);

			Assert::AreEqual(0, reinterpret_cast<int>(buf));
			Assert::AreEqual(0, static_cast<int>(size));

			buf = fb1.getBuffer(nullptr);

			Assert::AreEqual(0, reinterpret_cast<int>(buf));
			Assert::AreEqual(S_OK, fb1.Initialize(L"d:\\SHARE\\pic\\02.jpg"));

			const int test_file_size = 339631;
			const unsigned __int32 test_file_crc32 = 0xD8135C08;
			DWORD Dtest_file_crc32 = 0xD8135C08;
			buf = fb1.getBuffer(&size);

			Assert::AreNotEqual(0, reinterpret_cast<int>(buf));
			Assert::AreEqual(test_file_size, static_cast<int>(size));
			Assert::AreEqual(test_file_crc32, crc32(0, buf, size));
			Assert::AreEqual(static_cast<int>(0xE0FFD8FF), *reinterpret_cast<const int *>(buf));
			Assert::AreEqual(static_cast<int>(0xD9), static_cast<int>(buf[test_file_size - 1]));

			buf = fb1.getBuffer(0);

			Assert::AreEqual(0, reinterpret_cast<int>(buf), 0);
			Assert::AreEqual(E_FAIL, fb1.Initialize(L"d:\\SHARE\\pic\\Thumbs.db"));

			buf = fb1.getBuffer(&size);

			Assert::AreNotEqual(0, reinterpret_cast<int>(buf));
			Assert::AreEqual(test_file_size, static_cast<int>(size));
			Assert::AreEqual(static_cast<int>(0xE0FFD8FF), *reinterpret_cast<const int*>(buf));
			Assert::AreEqual(static_cast<int>(0xD9), static_cast<int>(buf[test_file_size - 1]));

			buf = fb1.getBuffer(0);
		}
	};

	TEST_CLASS(ImageStoreUnitTest)
	{
	public:

		TEST_METHOD(ImageStoreInit)
		{
			ImageStore is1;
			Assert::AreEqual(S_OK, is1.Initialize());
		}
	};
}
