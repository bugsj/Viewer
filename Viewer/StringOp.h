#pragma once

#include <vector>
#include <numeric>
#include <cstdlib>

namespace JViewer {
	namespace Tools {

		template<class T>
		size_t nulltermlen(const T* str)
		{
			size_t n = 0;
			while (*str++ != static_cast<T>(0)) {
				++n;
			}
			return n;
		}

		template<class T>
		size_t StringAppend(std::vector<T>* str, const std::vector<const T*>& ss)
		{
			size_t slen = str->empty() ? 0 : nulltermlen(str->data());
			size_t cnt = ss.size();
			std::vector<size_t> sslen(cnt);
			for (size_t i = 0; i < cnt; ++i) {
				sslen[i] = nulltermlen(ss[i]);
			}
			str->resize(std::accumulate(sslen.begin(), sslen.end(), slen) + 1);
			T* dst = str->data() + slen;
			for (size_t i = 0; i < cnt; ++i) {
				memcpy(dst, ss[i], sizeof(*dst) * sslen[i]);
				dst += sslen[i];
			}
			*dst = static_cast<T>(0);

			return nulltermlen(str->data());
		}

		template<class T> inline size_t StringAppend(std::vector<T>* str, const T* s1) {
			std::vector<const T*> ss = { s1 };
			return StringAppend(str, ss);
		}

		template<class T> inline size_t StringAppend(std::vector<T>* str, const T* s1, const T* s2) {
			std::vector<const T*> ss = { s1, s2 };
			return StringAppend(str, ss);
		}

		template<class T> inline size_t StringAppend(std::vector<T>* str, const T* s1, const T* s2, const T* s3) {
			std::vector<const T*> ss = { s1, s2, s3 };
			return StringAppend(str, ss);
		}

		template<class T> inline size_t StringAppend(std::vector<T>* str, const T* s1, const T* s2, const T* s3, const T* s4) {
			std::vector<const T*> ss = { s1, s2, s3, s4 };
			return StringAppend(str, ss);
		}

		template<class T> inline size_t StringAppend(std::vector<T>* str, const T* s1, const T* s2, const T* s3, const T* s4, const T* s5) {
			std::vector<const T*> ss = { s1, s2, s3, s4, s5 };
			return StringAppend(str, ss);
		}

		template<class T> inline size_t StringAppend(std::vector<T>* str, const T* s1, const T* s2, const T* s3, const T* s4, const T* s5, const T* s6) {
			std::vector<const T*> ss = { s1, s2, s3, s4, s5, s6 };
			return StringAppend(str, ss);
		}

	}
}
