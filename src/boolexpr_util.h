#pragma once

#include <boolexpr/boolexpr.h>

#include <array>
#include <bitset>
#include <functional>

namespace boolexpr
{
	template<typename T>
	constexpr std::array<boolexpr::bx_t, sizeof(T) * 8> unpackbits(const T & n) {
		constexpr size_t N = sizeof(T) * 8;
		std::bitset<N> b(n);
		std::array<boolexpr::bx_t, N> r;

		for (size_t i = 0; i < N; i++) {
			r[N - i - 1] = b[i] ? boolexpr::bx_t(boolexpr::one()) : boolexpr::bx_t(boolexpr::zero());
		}

		return r;
	}
}
