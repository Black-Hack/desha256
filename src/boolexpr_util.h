#pragma once

#include <boolexpr/boolexpr.h>

#include <array>
#include <bitset>
#include <functional>

namespace boolexpr
{
	static const bx_t zero_ = zero()->shared_from_this(), one_ = one()->shared_from_this();

	template<typename T>
	constexpr std::array<bx_t, sizeof(T) * 8>
		unpackbits(const T & n)
	{
		constexpr size_t N = sizeof(T) * 8;
		std::bitset<N> b(n);
		std::array<bx_t, N> r;

		for (size_t i = 0; i < N; i++)
		{
			r[N - i - 1] = b[i] ? one_ : zero_;
		}

		return r;
	}
}
