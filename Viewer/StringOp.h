#pragma once

#include <vector>
#include <numeric>
#include <cstdlib>
#include <initializer_list>

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
			size_t cnt = ss.size();
			std::vector<size_t> sslen(cnt);

			auto ssi = ss.begin();
			for (auto& sslen_i : sslen) {
				sslen_i = nulltermlen(*ssi++);
			}

			size_t slen = str->empty() ? 0 : nulltermlen(str->data());
			size_t tlen = std::accumulate(sslen.begin(), sslen.end(), slen);
			str->resize(tlen + 1);

			T* dst = str->data() + slen;

			ssi = ss.begin();
			for (auto& sslen_i : sslen) {
				std::memcpy(dst, *ssi++, sizeof(*dst) * sslen_i);
				dst += sslen_i;
			}
			*dst = static_cast<T>(0);

			return tlen;
		}

		template<class T> inline size_t StringAppend(std::vector<T>* str, const T* s1) {
			return StringAppend(str, std::vector<const T*>{ s1 });
		}

	}
}
