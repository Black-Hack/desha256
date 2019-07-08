#ifndef DESHA256_BIT_H_
#define DESHA256_BIT_H_

template <typename T>
class Bit {
public:
	Bit() : val() {}
	Bit(const T& val) : val(val) {}

	const T& value() const {
		return val;
	}

	static T raw_zero() { return false; }
	static T raw_one() { return true; }

	static T raw_not(const T& a) { return !a; }
	static T raw_and(const T& a, const T& b) { return a & b; }
	static T raw_or(const T& a, const T& b) { return a | b; }
	static T raw_xor(const T& a, const T& b) { return a ^ b; }

	static T raw_nand(const T& a, const T& b) { return raw_not(raw_and(a, b)); }
	static T raw_nor(const T& a, const T& b) { return raw_not(raw_or(a, b)); }
	static T raw_xnor(const T& a, const T& b) { return raw_not(raw_xor(a, b)); }

	static T raw_impl(const T& p, const T& q) { return raw_or(raw_not(p), q); }
	static T raw_ite(const T& s, const T& d1, const T& d0) { return raw_and(raw_or(raw_not(s), d1), raw_or(s, d0)); }

	static Bit<T> zero() { return raw_zero(); }
	static Bit<T> one() { return raw_one(); }

	static Bit<T> not_(const Bit<T>& a) {
		T r = raw_not(a.val);
		return r;
	}
	static Bit<T> and_(const Bit<T>& a, const Bit<T>& b) { return raw_and(a.val, b.val); }
	static Bit<T> or_(const Bit<T>& a, const Bit<T>& b) { return raw_or(a.val, b.val); }
	static Bit<T> xor_(const Bit<T>& a, const Bit<T>& b) { return raw_xor(a.val, b.val); }

	static Bit<T> nand(const Bit<T>& a, const Bit<T>& b) { return raw_nand(a.val, b.val); }
	static Bit<T> nor(const Bit<T>& a, const Bit<T>& b) { return raw_nor(a.val, b.val); }
	static Bit<T> xnor(const Bit<T>& a, const Bit<T>& b) { return raw_xnor(a.val, b.val); }

	static Bit<T> impl(const Bit<T>& p, const Bit<T>& q) { return raw_impl(p.val, q.val); }
	static Bit<T> ite(const Bit<T>& s, const Bit<T>& d1, const Bit<T>& d0) { return raw_ite(s.val, d1.val, d0.val); }

	Bit<T> operator~() const { return raw_not(val); }
	Bit<T> operator&(const Bit<T>& other) const { return raw_and(val, other.val); }
	Bit<T> operator|(const Bit<T>& other) const { return raw_or(val, other.val); }
	Bit<T> operator^(const Bit<T>& other) const { return raw_xor(val, other.val); }

	Bit<T>& operator&=(const Bit<T>& other) {
		val = raw_and(val, other.val);
		return *this;
	}
	Bit<T>& operator|=(const Bit<T>& other) {
		val = raw_or(val, other.val);
		return *this;
	}
	Bit<T>& operator^=(const Bit<T>& other) {
		val = raw_xor(val, other.val);
		return *this;
	}

private:
	T val;
};

#endif  // !DESHA256_BIT_H_
