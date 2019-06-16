#pragma once

#include "boolexpr_util.h"
#include "word.h"

#include <algorithm>

static constexpr std::array<uint32_t, 64> K = {
	0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
	0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
	0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
	0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
	0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
	0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
	0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
	0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
};

word<32> Ch(word<32> x, word<32> y, word<32> z) { return z ^ (x & (y ^ z)); }
word<32> Maj(word<32> x, word<32> y, word<32> z) { return (x & y) | (z & (x | y)); }
word<32> Sigma0(word<32> x) { return RotR(x, 2) ^ RotR(x, 13) ^ RotR(x, 22); }
word<32> Sigma1(word<32> x) { return RotR(x, 6) ^ RotR(x, 11) ^ RotR(x, 25); }
word<32> sigma0(word<32> x) { return RotR(x, 7) ^ RotR(x, 18) ^ ShR(x, 3); }
word<32> sigma1(word<32> x) { return RotR(x, 17) ^ RotR(x, 19) ^ ShR(x, 10); }

template<size_t N>
static std::array<boolexpr::bx_t, ((N + 65) / 512 + ((N + 65) % 512 == 0 ? 0 : 1)) * 512>
pad(const std::array<boolexpr::bx_t, N> & m) {
	std::array<boolexpr::bx_t, ((N + 65) / 512 + ((N + 65) % 512 == 0 ? 0 : 1)) * 512> r;
	std::copy(m.begin(), m.end(), r.begin());

	r[m.size()] = bit_one<boolexpr::bx_t>();
	std::fill(r.begin() + m.size() + 1, r.end() - 64, boolexpr::zero());

	word<64> len = boolexpr::unpackbits((uint64_t)N);
	std::copy_n(len.begin(), 64, r.end() - 64);

	return r;
}

void Round(word<32> a, word<32> b, word<32> c, word<32> & d, word<32> e, word<32> f, word<32> g, word<32> & h, word<32> k)
{
	word<32> t1 = h + Sigma1(e) + Ch(e, f, g) + k;
	word<32> t2 = Sigma0(a) + Maj(a, b, c);
	d = d + t1;
	h = t1 + t2;
}

void Transform(std::array<word<32>, 8> & s, std::array<word<32>, 16> & chunk)
{
	using boolexpr::unpackbits;

	word<32> a = s[0], b = s[1], c = s[2], d = s[3], e = s[4], f = s[5], g = s[6], h = s[7];
	word<32> w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, wa, wb, wc, wd, we, wf;

	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0x428a2f98u) + (w0 = chunk[0x0]));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0x71374491u) + (w1 = chunk[0x1]));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0xb5c0fbcfu) + (w2 = chunk[0x2]));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0xe9b5dba5u) + (w3 = chunk[0x3]));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0x3956c25bu) + (w4 = chunk[0x4]));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0x59f111f1u) + (w5 = chunk[0x5]));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0x923f82a4u) + (w6 = chunk[0x6]));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0xab1c5ed5u) + (w7 = chunk[0x7]));
	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0xd807aa98u) + (w8 = chunk[0x8]));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0x12835b01u) + (w9 = chunk[0x9]));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0x243185beu) + (wa = chunk[0xa]));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0x550c7dc3u) + (wb = chunk[0xb]));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0x72be5d74u) + (wc = chunk[0xc]));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0x80deb1feu) + (wd = chunk[0xd]));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0x9bdc06a7u) + (we = chunk[0xe]));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0xc19bf174u) + (wf = chunk[0xf]));

	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0xe49b69c1u) + (w0 += sigma1(we) + w9 + sigma0(w1)));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0xefbe4786u) + (w1 += sigma1(wf) + wa + sigma0(w2)));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0x0fc19dc6u) + (w2 += sigma1(w0) + wb + sigma0(w3)));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0x240ca1ccu) + (w3 += sigma1(w1) + wc + sigma0(w4)));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0x2de92c6fu) + (w4 += sigma1(w2) + wd + sigma0(w5)));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0x4a7484aau) + (w5 += sigma1(w3) + we + sigma0(w6)));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0x5cb0a9dcu) + (w6 += sigma1(w4) + wf + sigma0(w7)));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0x76f988dau) + (w7 += sigma1(w5) + w0 + sigma0(w8)));
	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0x983e5152u) + (w8 += sigma1(w6) + w1 + sigma0(w9)));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0xa831c66du) + (w9 += sigma1(w7) + w2 + sigma0(wa)));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0xb00327c8u) + (wa += sigma1(w8) + w3 + sigma0(wb)));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0xbf597fc7u) + (wb += sigma1(w9) + w4 + sigma0(wc)));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0xc6e00bf3u) + (wc += sigma1(wa) + w5 + sigma0(wd)));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0xd5a79147u) + (wd += sigma1(wb) + w6 + sigma0(we)));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0x06ca6351u) + (we += sigma1(wc) + w7 + sigma0(wf)));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0x14292967u) + (wf += sigma1(wd) + w8 + sigma0(w0)));

	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0x27b70a85u) + (w0 += sigma1(we) + w9 + sigma0(w1)));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0x2e1b2138u) + (w1 += sigma1(wf) + wa + sigma0(w2)));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0x4d2c6dfcu) + (w2 += sigma1(w0) + wb + sigma0(w3)));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0x53380d13u) + (w3 += sigma1(w1) + wc + sigma0(w4)));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0x650a7354u) + (w4 += sigma1(w2) + wd + sigma0(w5)));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0x766a0abbu) + (w5 += sigma1(w3) + we + sigma0(w6)));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0x81c2c92eu) + (w6 += sigma1(w4) + wf + sigma0(w7)));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0x92722c85u) + (w7 += sigma1(w5) + w0 + sigma0(w8)));
	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0xa2bfe8a1u) + (w8 += sigma1(w6) + w1 + sigma0(w9)));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0xa81a664bu) + (w9 += sigma1(w7) + w2 + sigma0(wa)));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0xc24b8b70u) + (wa += sigma1(w8) + w3 + sigma0(wb)));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0xc76c51a3u) + (wb += sigma1(w9) + w4 + sigma0(wc)));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0xd192e819u) + (wc += sigma1(wa) + w5 + sigma0(wd)));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0xd6990624u) + (wd += sigma1(wb) + w6 + sigma0(we)));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0xf40e3585u) + (we += sigma1(wc) + w7 + sigma0(wf)));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0x106aa070u) + (wf += sigma1(wd) + w8 + sigma0(w0)));

	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0x19a4c116u) + (w0 += sigma1(we) + w9 + sigma0(w1)));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0x1e376c08u) + (w1 += sigma1(wf) + wa + sigma0(w2)));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0x2748774cu) + (w2 += sigma1(w0) + wb + sigma0(w3)));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0x34b0bcb5u) + (w3 += sigma1(w1) + wc + sigma0(w4)));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0x391c0cb3u) + (w4 += sigma1(w2) + wd + sigma0(w5)));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0x4ed8aa4au) + (w5 += sigma1(w3) + we + sigma0(w6)));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0x5b9cca4fu) + (w6 += sigma1(w4) + wf + sigma0(w7)));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0x682e6ff3u) + (w7 += sigma1(w5) + w0 + sigma0(w8)));
	Round(a, b, c, d, e, f, g, h, unpackbits<uint32_t>(0x748f82eeu) + (w8 += sigma1(w6) + w1 + sigma0(w9)));
	Round(h, a, b, c, d, e, f, g, unpackbits<uint32_t>(0x78a5636fu) + (w9 += sigma1(w7) + w2 + sigma0(wa)));
	Round(g, h, a, b, c, d, e, f, unpackbits<uint32_t>(0x84c87814u) + (wa += sigma1(w8) + w3 + sigma0(wb)));
	Round(f, g, h, a, b, c, d, e, unpackbits<uint32_t>(0x8cc70208u) + (wb += sigma1(w9) + w4 + sigma0(wc)));
	Round(e, f, g, h, a, b, c, d, unpackbits<uint32_t>(0x90befffau) + (wc += sigma1(wa) + w5 + sigma0(wd)));
	Round(d, e, f, g, h, a, b, c, unpackbits<uint32_t>(0xa4506cebu) + (wd += sigma1(wb) + w6 + sigma0(we)));
	Round(c, d, e, f, g, h, a, b, unpackbits<uint32_t>(0xbef9a3f7u) + (we + sigma1(wc) + w7 + sigma0(wf)));
	Round(b, c, d, e, f, g, h, a, unpackbits<uint32_t>(0xc67178f2u) + (wf + sigma1(wd) + w8 + sigma0(w0)));

	s[0] += a;
	s[1] += b;
	s[2] += c;
	s[3] += d;
	s[4] += e;
	s[5] += f;
	s[6] += g;
	s[7] += h;
}

template<size_t N>
std::array<boolexpr::bx_t, 256>
sha256(const std::array<boolexpr::bx_t, N> & m) {
	using boolexpr::unpackbits;

	const auto a = pad(m);

	std::array<word<32>, 8> s = {
		unpackbits<uint32_t>(0x6a09e667u),
		unpackbits<uint32_t>(0xbb67ae85u),
		unpackbits<uint32_t>(0x3c6ef372u),
		unpackbits<uint32_t>(0xa54ff53au),
		unpackbits<uint32_t>(0x510e527fu),
		unpackbits<uint32_t>(0x9b05688cu),
		unpackbits<uint32_t>(0x1f83d9abu),
		unpackbits<uint32_t>(0x5be0cd19u)
	};


	for (size_t i = 0; i < a.size(); i += 512)
	{
		std::array<word<32>, 16>& w = *(std::array<word<32>, 16>*)&(a[i]);
		Transform(s, w);
	}

	std::array<boolexpr::bx_t, 256>& r = *(std::array<boolexpr::bx_t, 256>*) & s;
	return r;
}
