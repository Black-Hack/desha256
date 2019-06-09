#pragma once

#include <boolexpr/boolexpr.h>

#include <array>
#include <algorithm>
#include <functional>

template<size_t N>
using word = std::array<boolexpr::bx_t, N>;

template<typename T> T bit_and(const T& a, const T& b) { return a & b; }
template<typename T> T bit_or(const T& a, const T& b) { return a | b; }
template<typename T> T bit_xor(const T& a, const T& b) { return a ^ b; }
template<typename T> T bit_not(const T& a) { return ~a; }
template<typename T> T bit_zero() { return 0; }
template<typename T> T bit_one() { return 1; }

template <>
boolexpr::bx_t bit_and(const boolexpr::bx_t& a, const boolexpr::bx_t& b) {
	return boolexpr::and_s({ a, b });
}
template <>
boolexpr::bx_t bit_or(const boolexpr::bx_t& a, const boolexpr::bx_t& b) {
	return boolexpr::or_s({ a, b });
}
template <>
boolexpr::bx_t bit_xor(const boolexpr::bx_t& a, const boolexpr::bx_t& b) {
	return boolexpr::xor_s({ a, b });
}
template <>
boolexpr::bx_t bit_not(const boolexpr::bx_t& a) {
	return (~a)->simplify();
}
template <>
boolexpr::bx_t bit_zero() {
	return boolexpr::zero();
}
template <>
boolexpr::bx_t bit_one() {
	return boolexpr::one();
}

template<typename T, size_t N>
constexpr std::array<T, N> operator&(const std::array<T, N>& a, const std::array<T, N>& b) {
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), b.begin(), r.begin(), bit_and<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> operator|(const std::array<T, N>& a, const std::array<T, N>& b) {
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), b.begin(), r.begin(), bit_or<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> operator^(const std::array<T, N>& a, const std::array<T, N>& b) {
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), b.begin(), r.begin(), bit_xor<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> operator~(const std::array<T, N>& a) {
	std::array<T, N> r;
	std::transform(a.begin(), a.end(), r.begin(), bit_not<T>);
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> operator+(const std::array<T, N>& a, const std::array<T, N>& b) {
	T c = bit_zero<T>();

	std::array<T, N> r;
	for (int i = N - 1; i >= 0; i--) {
		T AxorB = bit_xor(a[i], b[i]);
		r[i] = bit_xor(AxorB, c);
		if (i != 0) {
			c = bit_or(bit_and(AxorB, c), bit_and(a[i], b[i]));
		}
	}
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> RotL(const std::array<T, N>& a, size_t n) {
	std::array<T, N> r;
	std::rotate_copy(a.begin(), a.begin() + n % N, a.end(), r.begin());
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> RotR(const std::array<T, N>& a, size_t n) {
	std::array<T, N> r;
	std::rotate_copy(a.begin(), a.end() - n % N, a.end(), r.begin());
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> ShL(const std::array<T, N>& a, size_t n) {
	std::array<T, N> r;
	std::copy(a.begin() + n, a.end(), r.begin());
	std::fill(r.begin() + n, r.end(), bit_zero<T>());
	return r;
}

template<typename T, size_t N>
constexpr std::array<T, N> ShR(const std::array<T, N>& a, size_t n) {
	std::array<T, N> r;
	std::fill_n(r.begin(), n, bit_zero<T>());
	std::copy_n(a.begin(), N - n, r.begin() + n);
	return r;
}
