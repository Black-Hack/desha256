#ifndef DESHA256_WORD_H_
#define DESHA256_WORD_H_

#include <algorithm>
#include <array>
#include <bitset>
#include <functional>

#include "bit.h"

template <typename T, size_t N>
class Word {
private:
	using bit_t = Bit<T>;
	using array_t = std::array<bit_t, N>;
	using word_t = Word<T, N>;

public:
	Word() {}
	Word(const word_t& other) : data_(other.data_) {}
	Word(const array_t& data) : data_(data) {}
	Word(const std::bitset<N>& bs) {
		for (size_t i = 0; i < N; i++) {
			data_[N - i - 1] = bs[i] ? bit_t::one() : bit_t::zero();
		}
	}
	Word(unsigned long long n) {
		std::bitset<N> bs(n);
		for (size_t i = 0; i < N; i++) {
			data_[N - i - 1] = bs[i] ? bit_t::one() : bit_t::zero();
		}
	}

	constexpr auto begin() noexcept { return data_.begin(); }
	constexpr auto begin() const noexcept { return data_.begin(); }
	constexpr auto end() noexcept { return data_.end(); }
	constexpr auto end() const noexcept { return data_.end(); }

	size_t size() const noexcept { return data_.size(); }

	Bit<T>& operator[](size_t i) { return data_[i]; }
	const Bit<T>& operator[](size_t i) const { return data_[i]; }

	constexpr bit_t* data() noexcept { return data_.data(); }
	constexpr const bit_t* data() const noexcept { return data_.data(); }

	static array_t raw_not(const array_t& a) { return transform(a, bit_t::not_); }
	static array_t raw_and(const array_t& a, const array_t& b) { return transform(a, b, bit_t::and_); }
	static array_t raw_or(const array_t& a, const array_t& b) { return transform(a, b, bit_t::or_); }
	static array_t raw_xor(const array_t& a, const array_t& b) { return transform(a, b, bit_t::xor_); }

	static array_t raw_nand(const array_t& a, const array_t& b) { return transform(a, b, bit_t::nand); }
	static array_t raw_nor(const array_t& a, const array_t& b) { return transform(a, b, bit_t::nor); }
	static array_t raw_xnor(const array_t& a, const array_t& b) { return transform(a, b, bit_t::xnor); }

	static array_t raw_impl(const array_t& p, const array_t& q) { return transform(p, q, bit_t::impl); }
	static array_t raw_ite(const bit_t& s, const array_t& d1, const array_t& d0) {
		using namespace std::placeholders;
		return transform(d1, d0, std::bind(bit_t::ite, std::cref(s), _1, _2));
	}

	static word_t not_(const word_t& a) { return transform(a, bit_t::not_); }
	static word_t and_(const word_t& a, const word_t& b) { return transform(a, b, bit_t::and_); }
	static word_t or_(const word_t& a, const word_t& b) { return transform(a, b, bit_t::or_); }
	static word_t xor_(const word_t& a, const word_t& b) { return transform(a, b, bit_t::xor_); }

	static word_t nand(const word_t& a, const word_t& b) { return transform(a, b, bit_t::nand); }
	static word_t nor(const word_t& a, const word_t& b) { return transform(a, b, bit_t::nor); }
	static word_t xnor(const word_t& a, const word_t& b) { return transform(a, b, bit_t::xnor); }

	static word_t impl(const word_t& p, const word_t& q) { return transform(p, q, bit_t::impl); }
	static word_t ite(const bit_t& s, const word_t& d1, const word_t& d0) {
		using namespace std::placeholders;
		return transform(d1, d0, std::bind(bit_t::ite, std::cref(s), _1, _2));
	}

	static void not_eq (word_t & a) { transform_self(a, bit_t::not_); }
	static void and_eq (word_t & a, const word_t& b) { transform_self(a, b, bit_t::and_); }
	static void or_eq (word_t & a, const word_t& b) { transform_self(a, b, bit_t::or_); }
	static void xor_eq (word_t & a, const word_t& b) { transform_self(a, b, bit_t::xor_); }

	word_t& operator=(const word_t& other) {
		data_ = other.data_;
		return *this;
	}
	word_t& operator=(const array_t& arr) {
		data_ = arr;
		return *this;
	}

	word_t operator~() const { return not_(*this); }
	word_t operator&(const word_t& other) const { return and_(*this, other); }
	word_t operator|(const word_t& other) const { return or_(*this, other); }
	word_t operator^(const word_t& other) const { return xor_(*this, other); }

	word_t& operator&=(const word_t& other) {
		and_eq (*this, other);
		return *this;
	}
	word_t& operator|=(const word_t& other) {
		or_eq (*this, other);
		return *this;
	}
	word_t& operator^=(const word_t& other) {
		xor_eq (*this, other);
		return *this;
	}

	word_t operator+(const word_t& other) const {
		bit_t c = bit_t::zero();

		word_t r;

		for (int i = N - 1; i >= 0; i--) {
			bit_t AxorB = data_[i] ^ other.data_[i];
			r.data_[i] = AxorB ^ c;
			if (i != 0) {
				c = (AxorB & c) | (data_[i] & other.data_[i]);
			}
		}

		return r;
	}
	word_t& operator+=(const word_t& other) {
		bit_t c = bit_t::zero();

		for (int i = N - 1; i >= 0; i--) {
			bit_t a = data_[i];
			bit_t AxorB = a ^ other.data_[i];
			data_[i] = AxorB ^ c;
			if (i != 0) {
				c = (AxorB & c) | (a & other.data_[i]);
			}
		}

		return *this;
	}

	word_t& operator++() {
		bit_t c = bit_t::one();

		for (int i = N - 1; i >= 0; i--) {
			bit_t a = data_[i];
			data_[i] ^= c;
			if (i != 0) {
				c &= a;
			}
		}

		return *this;
	}

	word_t operator-() const {
		word_t r;

		bit_t c = bit_t::one();

		for (int i = N - 1; i >= 0; i--) {
			bit_t a = ~data_[i];
			r.data_[i] = a ^ c;
			if (i != 0) {
				c &= a;
			}
		}
		return r;
	}

	word_t operator-(const word_t& other) const {
		return *this + (-other);
	}

	word_t operator>>(size_t n) const {
		word_t r;
		std::copy_n(begin(), N - n, r.begin() + n);
		std::fill_n(r.begin(), n, bit_t::zero());
		return r;
	}
	word_t& operator>>=(size_t n) {
		std::move_backward(begin(), end() - n, end());
		std::fill_n(begin(), n, bit_t::zero());
		return *this;
	}

	word_t operator<<(size_t n) const {
		word_t r;
		std::copy(begin() + n, end(), r.begin());
		std::fill(r.end() - n, r.end(), bit_t::zero());
		return r;
	}
	word_t& operator<<=(size_t n) {
		std::move(begin() + n, end(), begin());
		std::fill(end() - n, end(), bit_t::zero());
		return *this;
	}

	word_t rot_r(size_t n) const {
		word_t r;
		std::rotate_copy(begin(), end() - n % N, end(), r.begin());
		return r;
	}
	word_t& rot_r_eq(size_t n) {
		std::rotate(begin(), end() - n % N, end());
		return *this;
	}

	word_t rot_l(size_t n) const {
		word_t r;
		std::rotate_copy(begin(), begin() + n % N, end(), r.begin());
		return r;
	}
	word_t& rot_l_eq(size_t n) {
		std::rotate(begin(), begin() + n % N, end());
		return *this;
	}

	~Word() {}

private:
	template <typename U, typename Fn>
	inline static U transform(const U& a, Fn fn) {
		U r;
		std::transform(std::begin(a), std::end(a), std::begin(r), fn);
		return r;
	}

	template <typename U, typename Fn>
	inline static U transform(const U& a, const U& b, Fn fn) {
		U r;
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(r), fn);
		return r;
	}

	template <typename U, typename Fn>
	inline static void transform_self(U& a, Fn fn) {
		std::transform(std::begin(a), std::end(a), std::begin(a), fn);
	}

	template <typename U, typename Fn>
	inline static void transform_self(U& a, const U& b, Fn fn) {
		std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(a), fn);
	}

private:
	std::array<Bit<T>, N> data_;
};

template <typename T, typename U>
constexpr Word<T, sizeof(U) * 8> unpackbits(const U& n) {
	constexpr size_t N = sizeof(U) * 8;
	std::bitset<N> b(n);
	Word<T, N> r;

	for (size_t i = 0; i < N; i++) {
		r[N - i - 1] = b[i] ? Bit<T>::one() : Bit<T>::zero();
	}

	return r;
}

template <typename T, size_t N>
Word<T, N> operator+(unsigned long long i, const Word<T, N> w) {
	return Word<T, N>(i) + w;
}

#endif  // !DESHA256_WORD_H_
