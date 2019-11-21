#include "pch.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CppUnitTest.h"
#include "crc32.h"

#include "../Viewer/StringOp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ToolsUnitTest
{
	TEST_CLASS(StringToolsTest)
	{
	public:

		TEST_METHOD(NullTerminateStringLength)
		{
			Assert::AreEqual(0ULL, JViewer::Tools::nulltermlen(""));
			Assert::AreEqual(1ULL, JViewer::Tools::nulltermlen("1"));
			Assert::AreEqual(9ULL, JViewer::Tools::nulltermlen("123456789"));
			Assert::AreEqual(6ULL, JViewer::Tools::nulltermlen("12345\n"));
			Assert::AreEqual(10ULL, JViewer::Tools::nulltermlen("12345\n7890"));
			Assert::AreEqual(4ULL, JViewer::Tools::nulltermlen("一二"));
			Assert::AreEqual(10ULL, JViewer::Tools::nulltermlen("这是一句话"));
			Assert::AreEqual(9ULL, JViewer::Tools::nulltermlen("这是1句话"));
			Assert::AreEqual(0ULL, JViewer::Tools::nulltermlen(L""));
			Assert::AreEqual(1ULL, JViewer::Tools::nulltermlen(L"1"));
			Assert::AreEqual(9ULL, JViewer::Tools::nulltermlen(L"123456789"));
			Assert::AreEqual(6ULL, JViewer::Tools::nulltermlen(L"12345\n"));
			Assert::AreEqual(10ULL, JViewer::Tools::nulltermlen(L"12345\n7890"));
			Assert::AreEqual(2ULL, JViewer::Tools::nulltermlen(L"一二"));
			Assert::AreEqual(5ULL, JViewer::Tools::nulltermlen(L"这是1句话"));
		}

		TEST_METHOD(StringToolsAppend)
		{
			std::vector<char> charbuf;
			std::vector<wchar_t> wcharbuf;
			JViewer::Tools::StringAppend(&charbuf, "test");
			Assert::AreEqual("test", charbuf.data());
			JViewer::Tools::StringAppend(&charbuf, "test");
			Assert::AreEqual("testtest", charbuf.data());
			JViewer::Tools::StringAppend(&charbuf, { "1", "2" });
			Assert::AreEqual("testtest12", charbuf.data());
			JViewer::Tools::StringAppend(&charbuf, { "1", "2", "3" });
			Assert::AreEqual("testtest12123", charbuf.data());
			JViewer::Tools::StringAppend(&charbuf, { "a", "b", "c", "d" });
			Assert::AreEqual("testtest12123abcd", charbuf.data());
			JViewer::Tools::StringAppend(&charbuf, { "hello", ", ", "the", " ", "world" });
			Assert::AreEqual("testtest12123abcdhello, the world", charbuf.data());
			JViewer::Tools::StringAppend(&charbuf, { "!!!", "hello", ", ", "the", " ", "world" });
			Assert::AreEqual("testtest12123abcdhello, the world!!!hello, the world", charbuf.data());

			JViewer::Tools::StringAppend(&wcharbuf, L"test");
			Assert::AreEqual(L"test", wcharbuf.data());
			JViewer::Tools::StringAppend(&wcharbuf, L"test");
			Assert::AreEqual(L"testtest", wcharbuf.data());
			JViewer::Tools::StringAppend(&wcharbuf, { L"1", L"2" });
			Assert::AreEqual(L"testtest12", wcharbuf.data());
			JViewer::Tools::StringAppend(&wcharbuf, { L"1", L"2", L"3" });
			Assert::AreEqual(L"testtest12123", wcharbuf.data());
			JViewer::Tools::StringAppend(&wcharbuf, { L"a", L"b", L"c", L"d" });
			Assert::AreEqual(L"testtest12123abcd", wcharbuf.data());
			JViewer::Tools::StringAppend(&wcharbuf, { L"hello", L", ", L"the", L" ", L"world" });
			Assert::AreEqual(L"testtest12123abcdhello, the world", wcharbuf.data());
			JViewer::Tools::StringAppend(&wcharbuf, { L"!!!", L"hello", L", ", L"the", L" ", L"world" });
			Assert::AreEqual(L"testtest12123abcdhello, the world!!!hello, the world", wcharbuf.data());
		}

	};
}
