#pragma once

#include "word.h"
#include "nested_array.h"

#include <algorithm>

template<typename T>
class Sha256
{
private:
	using word = std::array<T, 32>;

public:
	Sha256()
	{
		Reset();
	}

	void Write(const T* data, size_t len)
	{
		const T* end = data + len;
		size_t bufsize = bits % 512;
		if (bufsize && bufsize + len >= 512)
		{
			// Fill the buffer, and process it.
			std::copy_n(data, 512 - bufsize, buf.flat.begin() + bufsize);
			bits += 512 - bufsize;
			data += 512 - bufsize;
			Transform(buf.nested);
			bufsize = 0;
		}
		if (end - data >= 512)
		{
			size_t blocks = (end - data) / 512;
			bits += 512 * blocks;
			while (blocks--)
			{
				Transform(data);
				data += 512;
			}
		}
		if (end > data)
		{
			// Fill the buffer with what remains.
			std::copy(data, end, buf.flat.begin() + bufsize);
			bits += end - data;
		}
	}

	template<typename Container>
	void Write(const Container& data)
	{
		Write(std::data(data), std::size(data));
	}

	void Write(const T& data)
	{
		Write(&data, 1);
	}

	std::array<T, 256> & Finalize()
	{
		const size_t len = bits;
		T zero = bit_zero<T>(), one = bit_one<T>();
		Write(&one, 1);
		while ((bits + 64) % 512 != 0)
		{
			Write(&zero, 1);
		}
		std::array<T, 64> sizedesc = unpackbits<T, uint64_t>(len);
		Write(sizedesc.data(), sizedesc.size());
		return s.flat;
	}

	void Reset()
	{
		bits = 0;
		s = std::array<word, 8>{
			unpackbits<T, uint32_t>(0x6a09e667u),
			unpackbits<T, uint32_t>(0xbb67ae85u),
			unpackbits<T, uint32_t>(0x3c6ef372u),
			unpackbits<T, uint32_t>(0xa54ff53au),
			unpackbits<T, uint32_t>(0x510e527fu),
			unpackbits<T, uint32_t>(0x9b05688cu),
			unpackbits<T, uint32_t>(0x1f83d9abu),
			unpackbits<T, uint32_t>(0x5be0cd19u),
		}
	}

private:
	static constexpr word Ch(const word& x, const word& y, const word& z) { return z ^ (x & (y ^ z)); }
	static constexpr word Maj(const word& x, const word& y, const word& z) { return (x & y) | (z & (x | y)); }
	static constexpr word Sigma0(const word& x) { return RotR(x, 2) ^ RotR(x, 13) ^ RotR(x, 22); }
	static constexpr word Sigma1(const word& x) { return RotR(x, 6) ^ RotR(x, 11) ^ RotR(x, 25); }
	static constexpr word sigma0(const word& x) { return RotR(x, 7) ^ RotR(x, 18) ^ ShR(x, 3); }
	static constexpr word sigma1(const word& x) { return RotR(x, 17) ^ RotR(x, 19) ^ ShR(x, 10); }

	/** One round of SHA-256. */
	static void Round(const word& a, const word& b, const word& c, word& d, const word& e, const word& f, const word& g, word& h, const word& k)
	{
		word t1 = h + Sigma1(e) + Ch(e, f, g) + k;
		word t2 = Sigma0(a) + Maj(a, b, c);
		d += t1;
		word x = t1 + t2;
		std::move(x.begin(), x.end(), h.begin());
		std::cout << 'R';
	}

	/** Perform a number of SHA-256 transformations, processing 64-byte chunks. */
	void Transform(const nested_array<T, 512, 16> & chunk)
	{
		word a = s(0), b = s(1), c = s(2), d = s(3), e = s(4), f = s(5), g = s(6), h = s(7);
		std::array<word, 16> w = chunk.nested;

		Round(a, b, c, d, e, f, g, h, 0x428a2f98 + w[0x0]);
		Round(h, a, b, c, d, e, f, g, 0x71374491 + w[0x1]);
		Round(g, h, a, b, c, d, e, f, 0xb5c0fbcf + w[0x2]);
		Round(f, g, h, a, b, c, d, e, 0xe9b5dba5 + w[0x3]);
		Round(e, f, g, h, a, b, c, d, 0x3956c25b + w[0x4]);
		Round(d, e, f, g, h, a, b, c, 0x59f111f1 + w[0x5]);
		Round(c, d, e, f, g, h, a, b, 0x923f82a4 + w[0x6]);
		Round(b, c, d, e, f, g, h, a, 0xab1c5ed5 + w[0x7]);
		Round(a, b, c, d, e, f, g, h, 0xd807aa98 + w[0x8]);
		Round(h, a, b, c, d, e, f, g, 0x12835b01 + w[0x9]);
		Round(g, h, a, b, c, d, e, f, 0x243185be + w[0xa]);
		Round(f, g, h, a, b, c, d, e, 0x550c7dc3 + w[0xb]);
		Round(e, f, g, h, a, b, c, d, 0x72be5d74 + w[0xc]);
		Round(d, e, f, g, h, a, b, c, 0x80deb1fe + w[0xd]);
		Round(c, d, e, f, g, h, a, b, 0x9bdc06a7 + w[0xe]);
		Round(b, c, d, e, f, g, h, a, 0xc19bf174 + w[0xf]);

		Round(a, b, c, d, e, f, g, h, 0xe49b69c1 + (w[0x0] += sigma1(w[0xe]) + w[0x9] + sigma0(w[0x1])));
		Round(h, a, b, c, d, e, f, g, 0xefbe4786 + (w[0x1] += sigma1(w[0xf]) + w[0xa] + sigma0(w[0x2])));
		Round(g, h, a, b, c, d, e, f, 0x0fc19dc6 + (w[0x2] += sigma1(w[0x0]) + w[0xb] + sigma0(w[0x3])));
		Round(f, g, h, a, b, c, d, e, 0x240ca1cc + (w[0x3] += sigma1(w[0x1]) + w[0xc] + sigma0(w[0x4])));
		Round(e, f, g, h, a, b, c, d, 0x2de92c6f + (w[0x4] += sigma1(w[0x2]) + w[0xd] + sigma0(w[0x5])));
		Round(d, e, f, g, h, a, b, c, 0x4a7484aa + (w[0x5] += sigma1(w[0x3]) + w[0xe] + sigma0(w[0x6])));
		Round(c, d, e, f, g, h, a, b, 0x5cb0a9dc + (w[0x6] += sigma1(w[0x4]) + w[0xf] + sigma0(w[0x7])));
		Round(b, c, d, e, f, g, h, a, 0x76f988da + (w[0x7] += sigma1(w[0x5]) + w[0x0] + sigma0(w[0x8])));
		Round(a, b, c, d, e, f, g, h, 0x983e5152 + (w[0x8] += sigma1(w[0x6]) + w[0x1] + sigma0(w[0x9])));
		Round(h, a, b, c, d, e, f, g, 0xa831c66d + (w[0x9] += sigma1(w[0x7]) + w[0x2] + sigma0(w[0xa])));
		Round(g, h, a, b, c, d, e, f, 0xb00327c8 + (w[0xa] += sigma1(w[0x8]) + w[0x3] + sigma0(w[0xb])));
		Round(f, g, h, a, b, c, d, e, 0xbf597fc7 + (w[0xb] += sigma1(w[0x9]) + w[0x4] + sigma0(w[0xc])));
		Round(e, f, g, h, a, b, c, d, 0xc6e00bf3 + (w[0xc] += sigma1(w[0xa]) + w[0x5] + sigma0(w[0xd])));
		Round(d, e, f, g, h, a, b, c, 0xd5a79147 + (w[0xd] += sigma1(w[0xb]) + w[0x6] + sigma0(w[0xe])));
		Round(c, d, e, f, g, h, a, b, 0x06ca6351 + (w[0xe] += sigma1(w[0xc]) + w[0x7] + sigma0(w[0xf])));
		Round(b, c, d, e, f, g, h, a, 0x14292967 + (w[0xf] += sigma1(w[0xd]) + w[0x8] + sigma0(w[0x0])));

		Round(a, b, c, d, e, f, g, h, 0x27b70a85 + (w[0x0] += sigma1(w[0xe]) + w[0x9] + sigma0(w[0x1])));
		Round(h, a, b, c, d, e, f, g, 0x2e1b2138 + (w[0x1] += sigma1(w[0xf]) + w[0xa] + sigma0(w[0x2])));
		Round(g, h, a, b, c, d, e, f, 0x4d2c6dfc + (w[0x2] += sigma1(w[0x0]) + w[0xb] + sigma0(w[0x3])));
		Round(f, g, h, a, b, c, d, e, 0x53380d13 + (w[0x3] += sigma1(w[0x1]) + w[0xc] + sigma0(w[0x4])));
		Round(e, f, g, h, a, b, c, d, 0x650a7354 + (w[0x4] += sigma1(w[0x2]) + w[0xd] + sigma0(w[0x5])));
		Round(d, e, f, g, h, a, b, c, 0x766a0abb + (w[0x5] += sigma1(w[0x3]) + w[0xe] + sigma0(w[0x6])));
		Round(c, d, e, f, g, h, a, b, 0x81c2c92e + (w[0x6] += sigma1(w[0x4]) + w[0xf] + sigma0(w[0x7])));
		Round(b, c, d, e, f, g, h, a, 0x92722c85 + (w[0x7] += sigma1(w[0x5]) + w[0x0] + sigma0(w[0x8])));
		Round(a, b, c, d, e, f, g, h, 0xa2bfe8a1 + (w[0x8] += sigma1(w[0x6]) + w[0x1] + sigma0(w[0x9])));
		Round(h, a, b, c, d, e, f, g, 0xa81a664b + (w[0x9] += sigma1(w[0x7]) + w[0x2] + sigma0(w[0xa])));
		Round(g, h, a, b, c, d, e, f, 0xc24b8b70 + (w[0xa] += sigma1(w[0x8]) + w[0x3] + sigma0(w[0xb])));
		Round(f, g, h, a, b, c, d, e, 0xc76c51a3 + (w[0xb] += sigma1(w[0x9]) + w[0x4] + sigma0(w[0xc])));
		Round(e, f, g, h, a, b, c, d, 0xd192e819 + (w[0xc] += sigma1(w[0xa]) + w[0x5] + sigma0(w[0xd])));
		Round(d, e, f, g, h, a, b, c, 0xd6990624 + (w[0xd] += sigma1(w[0xb]) + w[0x6] + sigma0(w[0xe])));
		Round(c, d, e, f, g, h, a, b, 0xf40e3585 + (w[0xe] += sigma1(w[0xc]) + w[0x7] + sigma0(w[0xf])));
		Round(b, c, d, e, f, g, h, a, 0x106aa070 + (w[0xf] += sigma1(w[0xd]) + w[0x8] + sigma0(w[0x0])));

		Round(a, b, c, d, e, f, g, h, 0x19a4c116 + (w[0x0] += sigma1(w[0xe]) + w[0x9] + sigma0(w[0x1])));
		Round(h, a, b, c, d, e, f, g, 0x1e376c08 + (w[0x1] += sigma1(w[0xf]) + w[0xa] + sigma0(w[0x2])));
		Round(g, h, a, b, c, d, e, f, 0x2748774c + (w[0x2] += sigma1(w[0x0]) + w[0xb] + sigma0(w[0x3])));
		Round(f, g, h, a, b, c, d, e, 0x34b0bcb5 + (w[0x3] += sigma1(w[0x1]) + w[0xc] + sigma0(w[0x4])));
		Round(e, f, g, h, a, b, c, d, 0x391c0cb3 + (w[0x4] += sigma1(w[0x2]) + w[0xd] + sigma0(w[0x5])));
		Round(d, e, f, g, h, a, b, c, 0x4ed8aa4a + (w[0x5] += sigma1(w[0x3]) + w[0xe] + sigma0(w[0x6])));
		Round(c, d, e, f, g, h, a, b, 0x5b9cca4f + (w[0x6] += sigma1(w[0x4]) + w[0xf] + sigma0(w[0x7])));
		Round(b, c, d, e, f, g, h, a, 0x682e6ff3 + (w[0x7] += sigma1(w[0x5]) + w[0x0] + sigma0(w[0x8])));
		Round(a, b, c, d, e, f, g, h, 0x748f82ee + (w[0x8] += sigma1(w[0x6]) + w[0x1] + sigma0(w[0x9])));
		Round(h, a, b, c, d, e, f, g, 0x78a5636f + (w[0x9] += sigma1(w[0x7]) + w[0x2] + sigma0(w[0xa])));
		Round(g, h, a, b, c, d, e, f, 0x84c87814 + (w[0xa] += sigma1(w[0x8]) + w[0x3] + sigma0(w[0xb])));
		Round(f, g, h, a, b, c, d, e, 0x8cc70208 + (w[0xb] += sigma1(w[0x9]) + w[0x4] + sigma0(w[0xc])));
		Round(e, f, g, h, a, b, c, d, 0x90befffa + (w[0xc] += sigma1(w[0xa]) + w[0x5] + sigma0(w[0xd])));
		Round(d, e, f, g, h, a, b, c, 0xa4506ceb + (w[0xd] += sigma1(w[0xb]) + w[0x6] + sigma0(w[0xe])));
		Round(c, d, e, f, g, h, a, b, 0xbef9a3f7 + (w[0xe] + sigma1(w[0xc]) + w[0x7] + sigma0(w[0xf])));
		Round(b, c, d, e, f, g, h, a, 0xc67178f2 + (w[0xf] + sigma1(w[0xd]) + w[0x8] + sigma0(w[0x0])));

		s.nested[0] += a;
		s.nested[1] += b;
		s.nested[2] += c;
		s.nested[3] += d;
		s.nested[4] += e;
		s.nested[5] += f;
		s.nested[6] += g;
		s.nested[7] += h;
	}

private:
	uint64_t bits;
	nested_array<T, 512, 16> buf;
	nested_array<T, 256, 8> s;

};
