#ifndef DESHA256_SHA256_H_
#define DESHA256_SHA256_H_

#include <algorithm>

#include "nested_container.h"
#include "word.h"

template <typename T>
class Sha256 {
private:
	using word_t = Word<T, 32>;
	using bit_t = Bit<T>;
	template <size_t N>
	using nested_word = NestedContainer<std::array<bit_t, N * 32>, std::array<word_t, N>>;

public:
	Sha256() {
		Reset();
	}

	void Write(const bit_t* data, size_t len) {
		const bit_t* end = data + len;
		size_t bufsize = bits_ % 512;
		if (bufsize && bufsize + len >= 512) {
			// Fill the buffer, and process it.
			std::copy_n(data, 512 - bufsize, buf_.as_flat().begin() + bufsize);
			bits_ += 512 - bufsize;
			data += 512 - bufsize;
			Transform();
			bufsize = 0;
		}
		if (end - data >= 512) {
			size_t blocks = (end - data) / 512;
			bits_ += 512 * blocks;
			while (blocks--) {
				std::copy_n(data, 512, buf_.as_flat().begin());
				Transform();
				data += 512;
			}
		}
		if (end > data) {
			// Fill the buffer with what remains.
			std::copy(data, end, buf_.as_flat().begin() + bufsize);
			bits_ += end - data;
		}
	}

	template <typename Container>
	void Write(const Container& data) {
		Write(std::data(data), std::size(data));
	}

	void Write(const bit_t& data) {
		Write(&data, 1);
	}

	std::array<Bit<T>, 256>& Finalize() {
		const size_t len = bits_;
		Bit<T> zero = Bit<T>::zero(), one = Bit<T>::one();

		Write(&one, 1);

		while ((bits_ + 64) % 512 != 0) {
			Write(&zero, 1);
		}

		Write(unpackbits<T, uint64_t>(len));

		return s_.as_flat();
	}

	void Reset() {
		bits_ = 0;
		s_.as_nested() = {
			0x6a09e667u,
			0xbb67ae85u,
			0x3c6ef372u,
			0xa54ff53au,
			0x510e527fu,
			0x9b05688cu,
			0x1f83d9abu,
			0x5be0cd19u,
		};
	}

	~Sha256() {}

private:
	static constexpr word_t Ch(const word_t& x, const word_t& y, const word_t& z) { return z ^ (x & (y ^ z)); }
	static constexpr word_t Maj(const word_t& x, const word_t& y, const word_t& z) { return (x & y) | (z & (x | y)); }
	static constexpr word_t Sigma0(const word_t& x) { return x.rot_r(2) ^ x.rot_r(13) ^ x.rot_r(22); }
	static constexpr word_t Sigma1(const word_t& x) { return x.rot_r(6) ^ x.rot_r(11) ^ x.rot_r(25); }
	static constexpr word_t sigma0(const word_t& x) { return x.rot_r(7) ^ x.rot_r(18) ^ (x >> 3); }
	static constexpr word_t sigma1(const word_t& x) { return x.rot_r(17) ^ x.rot_r(19) ^ (x >> 10); }

	/** One round of SHA-256. */
	static void Round(const word_t& a, const word_t& b, const word_t& c, word_t& d,
					  const word_t& e, const word_t& f, const word_t& g, word_t& h, const word_t& k) {
		word_t t1 = h + Sigma1(e) + Ch(e, f, g) + k;
		word_t t2 = Sigma0(a) + Maj(a, b, c);
		d += t1;
		h = t1 + t2;
	}

	/** Perform a number of SHA-256 transformations, processing 64-byte chunks. */
	void Transform() {
		word_t a = s_(0), b = s_(1), c = s_(2), d = s_(3), e = s_(4), f = s_(5), g = s_(6), h = s_(7);

		Round(a, b, c, d, e, f, g, h, 0x428a2f98 + buf_(0x0));
		Round(h, a, b, c, d, e, f, g, 0x71374491 + buf_(0x1));
		Round(g, h, a, b, c, d, e, f, 0xb5c0fbcf + buf_(0x2));
		Round(f, g, h, a, b, c, d, e, 0xe9b5dba5 + buf_(0x3));
		Round(e, f, g, h, a, b, c, d, 0x3956c25b + buf_(0x4));
		Round(d, e, f, g, h, a, b, c, 0x59f111f1 + buf_(0x5));
		Round(c, d, e, f, g, h, a, b, 0x923f82a4 + buf_(0x6));
		Round(b, c, d, e, f, g, h, a, 0xab1c5ed5 + buf_(0x7));
		Round(a, b, c, d, e, f, g, h, 0xd807aa98 + buf_(0x8));
		Round(h, a, b, c, d, e, f, g, 0x12835b01 + buf_(0x9));
		Round(g, h, a, b, c, d, e, f, 0x243185be + buf_(0xa));
		Round(f, g, h, a, b, c, d, e, 0x550c7dc3 + buf_(0xb));
		Round(e, f, g, h, a, b, c, d, 0x72be5d74 + buf_(0xc));
		Round(d, e, f, g, h, a, b, c, 0x80deb1fe + buf_(0xd));
		Round(c, d, e, f, g, h, a, b, 0x9bdc06a7 + buf_(0xe));
		Round(b, c, d, e, f, g, h, a, 0xc19bf174 + buf_(0xf));

		Round(a, b, c, d, e, f, g, h, 0xe49b69c1 + (buf_(0x0) += sigma1(buf_(0xe)) + buf_(0x9) + sigma0(buf_(0x1))));
		Round(h, a, b, c, d, e, f, g, 0xefbe4786 + (buf_(0x1) += sigma1(buf_(0xf)) + buf_(0xa) + sigma0(buf_(0x2))));
		Round(g, h, a, b, c, d, e, f, 0x0fc19dc6 + (buf_(0x2) += sigma1(buf_(0x0)) + buf_(0xb) + sigma0(buf_(0x3))));
		Round(f, g, h, a, b, c, d, e, 0x240ca1cc + (buf_(0x3) += sigma1(buf_(0x1)) + buf_(0xc) + sigma0(buf_(0x4))));
		Round(e, f, g, h, a, b, c, d, 0x2de92c6f + (buf_(0x4) += sigma1(buf_(0x2)) + buf_(0xd) + sigma0(buf_(0x5))));
		Round(d, e, f, g, h, a, b, c, 0x4a7484aa + (buf_(0x5) += sigma1(buf_(0x3)) + buf_(0xe) + sigma0(buf_(0x6))));
		Round(c, d, e, f, g, h, a, b, 0x5cb0a9dc + (buf_(0x6) += sigma1(buf_(0x4)) + buf_(0xf) + sigma0(buf_(0x7))));
		Round(b, c, d, e, f, g, h, a, 0x76f988da + (buf_(0x7) += sigma1(buf_(0x5)) + buf_(0x0) + sigma0(buf_(0x8))));
		Round(a, b, c, d, e, f, g, h, 0x983e5152 + (buf_(0x8) += sigma1(buf_(0x6)) + buf_(0x1) + sigma0(buf_(0x9))));
		Round(h, a, b, c, d, e, f, g, 0xa831c66d + (buf_(0x9) += sigma1(buf_(0x7)) + buf_(0x2) + sigma0(buf_(0xa))));
		Round(g, h, a, b, c, d, e, f, 0xb00327c8 + (buf_(0xa) += sigma1(buf_(0x8)) + buf_(0x3) + sigma0(buf_(0xb))));
		Round(f, g, h, a, b, c, d, e, 0xbf597fc7 + (buf_(0xb) += sigma1(buf_(0x9)) + buf_(0x4) + sigma0(buf_(0xc))));
		Round(e, f, g, h, a, b, c, d, 0xc6e00bf3 + (buf_(0xc) += sigma1(buf_(0xa)) + buf_(0x5) + sigma0(buf_(0xd))));
		Round(d, e, f, g, h, a, b, c, 0xd5a79147 + (buf_(0xd) += sigma1(buf_(0xb)) + buf_(0x6) + sigma0(buf_(0xe))));
		Round(c, d, e, f, g, h, a, b, 0x06ca6351 + (buf_(0xe) += sigma1(buf_(0xc)) + buf_(0x7) + sigma0(buf_(0xf))));
		Round(b, c, d, e, f, g, h, a, 0x14292967 + (buf_(0xf) += sigma1(buf_(0xd)) + buf_(0x8) + sigma0(buf_(0x0))));

		Round(a, b, c, d, e, f, g, h, 0x27b70a85 + (buf_(0x0) += sigma1(buf_(0xe)) + buf_(0x9) + sigma0(buf_(0x1))));
		Round(h, a, b, c, d, e, f, g, 0x2e1b2138 + (buf_(0x1) += sigma1(buf_(0xf)) + buf_(0xa) + sigma0(buf_(0x2))));
		Round(g, h, a, b, c, d, e, f, 0x4d2c6dfc + (buf_(0x2) += sigma1(buf_(0x0)) + buf_(0xb) + sigma0(buf_(0x3))));
		Round(f, g, h, a, b, c, d, e, 0x53380d13 + (buf_(0x3) += sigma1(buf_(0x1)) + buf_(0xc) + sigma0(buf_(0x4))));
		Round(e, f, g, h, a, b, c, d, 0x650a7354 + (buf_(0x4) += sigma1(buf_(0x2)) + buf_(0xd) + sigma0(buf_(0x5))));
		Round(d, e, f, g, h, a, b, c, 0x766a0abb + (buf_(0x5) += sigma1(buf_(0x3)) + buf_(0xe) + sigma0(buf_(0x6))));
		Round(c, d, e, f, g, h, a, b, 0x81c2c92e + (buf_(0x6) += sigma1(buf_(0x4)) + buf_(0xf) + sigma0(buf_(0x7))));
		Round(b, c, d, e, f, g, h, a, 0x92722c85 + (buf_(0x7) += sigma1(buf_(0x5)) + buf_(0x0) + sigma0(buf_(0x8))));
		Round(a, b, c, d, e, f, g, h, 0xa2bfe8a1 + (buf_(0x8) += sigma1(buf_(0x6)) + buf_(0x1) + sigma0(buf_(0x9))));
		Round(h, a, b, c, d, e, f, g, 0xa81a664b + (buf_(0x9) += sigma1(buf_(0x7)) + buf_(0x2) + sigma0(buf_(0xa))));
		Round(g, h, a, b, c, d, e, f, 0xc24b8b70 + (buf_(0xa) += sigma1(buf_(0x8)) + buf_(0x3) + sigma0(buf_(0xb))));
		Round(f, g, h, a, b, c, d, e, 0xc76c51a3 + (buf_(0xb) += sigma1(buf_(0x9)) + buf_(0x4) + sigma0(buf_(0xc))));
		Round(e, f, g, h, a, b, c, d, 0xd192e819 + (buf_(0xc) += sigma1(buf_(0xa)) + buf_(0x5) + sigma0(buf_(0xd))));
		Round(d, e, f, g, h, a, b, c, 0xd6990624 + (buf_(0xd) += sigma1(buf_(0xb)) + buf_(0x6) + sigma0(buf_(0xe))));
		Round(c, d, e, f, g, h, a, b, 0xf40e3585 + (buf_(0xe) += sigma1(buf_(0xc)) + buf_(0x7) + sigma0(buf_(0xf))));
		Round(b, c, d, e, f, g, h, a, 0x106aa070 + (buf_(0xf) += sigma1(buf_(0xd)) + buf_(0x8) + sigma0(buf_(0x0))));

		Round(a, b, c, d, e, f, g, h, 0x19a4c116 + (buf_(0x0) += sigma1(buf_(0xe)) + buf_(0x9) + sigma0(buf_(0x1))));
		Round(h, a, b, c, d, e, f, g, 0x1e376c08 + (buf_(0x1) += sigma1(buf_(0xf)) + buf_(0xa) + sigma0(buf_(0x2))));
		Round(g, h, a, b, c, d, e, f, 0x2748774c + (buf_(0x2) += sigma1(buf_(0x0)) + buf_(0xb) + sigma0(buf_(0x3))));
		Round(f, g, h, a, b, c, d, e, 0x34b0bcb5 + (buf_(0x3) += sigma1(buf_(0x1)) + buf_(0xc) + sigma0(buf_(0x4))));
		Round(e, f, g, h, a, b, c, d, 0x391c0cb3 + (buf_(0x4) += sigma1(buf_(0x2)) + buf_(0xd) + sigma0(buf_(0x5))));
		Round(d, e, f, g, h, a, b, c, 0x4ed8aa4a + (buf_(0x5) += sigma1(buf_(0x3)) + buf_(0xe) + sigma0(buf_(0x6))));
		Round(c, d, e, f, g, h, a, b, 0x5b9cca4f + (buf_(0x6) += sigma1(buf_(0x4)) + buf_(0xf) + sigma0(buf_(0x7))));
		Round(b, c, d, e, f, g, h, a, 0x682e6ff3 + (buf_(0x7) += sigma1(buf_(0x5)) + buf_(0x0) + sigma0(buf_(0x8))));
		Round(a, b, c, d, e, f, g, h, 0x748f82ee + (buf_(0x8) += sigma1(buf_(0x6)) + buf_(0x1) + sigma0(buf_(0x9))));
		Round(h, a, b, c, d, e, f, g, 0x78a5636f + (buf_(0x9) += sigma1(buf_(0x7)) + buf_(0x2) + sigma0(buf_(0xa))));
		Round(g, h, a, b, c, d, e, f, 0x84c87814 + (buf_(0xa) += sigma1(buf_(0x8)) + buf_(0x3) + sigma0(buf_(0xb))));
		Round(f, g, h, a, b, c, d, e, 0x8cc70208 + (buf_(0xb) += sigma1(buf_(0x9)) + buf_(0x4) + sigma0(buf_(0xc))));
		Round(e, f, g, h, a, b, c, d, 0x90befffa + (buf_(0xc) += sigma1(buf_(0xa)) + buf_(0x5) + sigma0(buf_(0xd))));
		Round(d, e, f, g, h, a, b, c, 0xa4506ceb + (buf_(0xd) += sigma1(buf_(0xb)) + buf_(0x6) + sigma0(buf_(0xe))));
		Round(c, d, e, f, g, h, a, b, 0xbef9a3f7 + (buf_(0xe) + sigma1(buf_(0xc)) + buf_(0x7) + sigma0(buf_(0xf))));
		Round(b, c, d, e, f, g, h, a, 0xc67178f2 + (buf_(0xf) + sigma1(buf_(0xd)) + buf_(0x8) + sigma0(buf_(0x0))));

		s_(0) += a;
		s_(1) += b;
		s_(2) += c;
		s_(3) += d;
		s_(4) += e;
		s_(5) += f;
		s_(6) += g;
		s_(7) += h;
	}

private:
	uint64_t bits_;
	nested_word<16> buf_;
	nested_word<8> s_;
};

#endif  // !DESHA256_SHA256_H_
