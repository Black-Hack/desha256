#ifndef DESHA256_BOOLEXPR_UTIL_H_
#define DESHA256_BOOLEXPR_UTIL_H_

#include <boolexpr/boolexpr.h>

#include "bit.h"

using boolexpr::bx_t;

template <>
bx_t Bit<bx_t>::raw_or(const bx_t& a, const bx_t& b) {
	return boolexpr::or_s({a, b});
}

template <>
bx_t Bit<bx_t>::raw_nor(const bx_t& a, const bx_t& b) {
	return boolexpr::nor_s({a, b});
}

template <>
bx_t Bit<bx_t>::raw_and(const bx_t& a, const bx_t& b) {
	return boolexpr::and_s({a, b});
}

template <>
bx_t Bit<bx_t>::raw_nand(const bx_t& a, const bx_t& b) {
	return boolexpr::nand_s({a, b});
}

template <>
bx_t Bit<bx_t>::raw_xor(const bx_t& a, const bx_t& b) {
	return boolexpr::xor_s({a, b});
}

template <>
bx_t Bit<bx_t>::raw_xnor(const bx_t& a, const bx_t& b) {
	return boolexpr::xnor_s({a, b});
}

template <>
bx_t Bit<bx_t>::raw_impl(const bx_t& a, const bx_t& b) {
	return boolexpr::impl_s(a, b);
}

template <>
bx_t Bit<bx_t>::raw_ite(const bx_t& s, const bx_t& d1, const bx_t& d0) {
	return boolexpr::ite_s(s, d1, d0);
}

template <>
bx_t Bit<bx_t>::raw_zero() {
	static bx_t zero_ = boolexpr::zero()->shared_from_this();
	return zero_;
}

template <>
bx_t Bit<bx_t>::raw_one() {
	static bx_t one_ = boolexpr::one()->shared_from_this();
	return one_;
}

#endif  // !DESHA256_BOOLEXPR_UTIL_H_
