#pragma once

#include <array>
#include <bitset>
#include <algorithm>

template<typename T> T bit_and(const T& a, const T& b) { return a & b; }
template<typename T> T bit_or(const T& a, const T& b) { return a | b; }
template<typename T> T bit_xor(const T& a, const T& b) { return a ^ b; }
template<typename T> T bit_not(const T& a) { return ~a; }
template<typename T> T bit_zero() { return false; }
template<typename T> T bit_one() { return true; }

template<typename T, typename U>
constexpr std::array<T, sizeof(U) * 8>
unpackbits(const U & n)
{
	constexpr size_t N = sizeof(U) * 8;
	std::bitset<N> b(n);
	std::array<T, N> r;

	for (size_t i = 0; i < N; i++)
	{
		r[N - i - 1] = b[i] ? bit_one<T>() : bit_zero<T>();
	}

	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N>
operator&(const std::array<T, N>& a, const std::array<T, N>& b)
{
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), b.begin(), r.begin(), bit_and<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N>
operator|(const std::array<T, N>& a, const std::array<T, N>& b)
{
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), b.begin(), r.begin(), bit_or<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N>
operator^(const std::array<T, N>& a, const std::array<T, N>& b)
{
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), b.begin(), r.begin(), bit_xor<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N>
operator~(const std::array<T, N>& a)
{
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), r.begin(), bit_not<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N>
operator+(const std::array<T, N>& a, const std::array<T, N>& b)
{
	T c = bit_zero<T>();

	std::array<T, N> r;
	for (int i = N - 1; i >= 0; i--)
	{
		T AxorB = bit_xor(a[i], b[i]);
		r[i] = bit_xor(AxorB, c);
		if (i != 0)
		{
			c = bit_or(bit_and(AxorB, c), bit_and(a[i], b[i]));
		}
	}
	return r;
}

template<typename T, typename U>
constexpr std::array<T, sizeof(U) * 8>
operator+(const std::array<T, sizeof(U) * 8> a, const U & b)
{
	return a + unpackbits<T, U>(b);
}

template<typename T, typename U>
constexpr std::array<T, sizeof(U) * 8>
operator+(const U & b, const std::array<T, sizeof(U) * 8> a)
{
	return a + unpackbits<T, U>(b);
}

template<typename T, size_t N>
std::array<T, N>&
operator+=(std::array<T, N>& a, const std::array<T, N>& b)
{
	a = a + b;
	return a;
}

template<typename T, size_t N>
constexpr std::array<T, N>
RotL(const std::array<T, N>& a, size_t n)
{
	std::array<T, N> r;
	std::rotate_copy(a.begin(), a.begin() + n % N, a.end(), r.begin());
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> RotR(const std::array<T, N>& a, size_t n)
{
	std::array<T, N> r;
	std::rotate_copy(a.begin(), a.end() - n % N, a.end(), r.begin());
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N>
ShL(const std::array<T, N>& a, size_t n)
{
	std::array<T, N> r;
	std::copy(a.begin() + n, a.end(), r.begin());
	std::fill(r.begin() + n, r.end(), bit_zero<T>());
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N>
ShR(const std::array<T, N>& a, size_t n)
{
	std::array<T, N> r;
	std::fill_n(r.begin(), n, bit_zero<T>());
	std::copy_n(a.begin(), N - n, r.begin() + n);
	return r;
}
