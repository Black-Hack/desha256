#pragma once

#include <boolexpr/boolexpr.h>

#include "word.h"

using boolexpr::bx_t;

template<>
bx_t bit_or(const bx_t& a, const bx_t& b)
{
	return boolexpr::or_s({ a, b });
}

template<>
bx_t bit_nor(const bx_t& a, const bx_t& b)
{
	return boolexpr::nor_s({ a, b });
}

template<>
bx_t bit_and(const bx_t& a, const bx_t& b)
{
	return boolexpr::and_s({ a, b });
}

template<>
bx_t bit_nand(const bx_t& a, const bx_t& b)
{
	return boolexpr::nand_s({ a, b });
}

template<>
bx_t bit_xor(const bx_t& a, const bx_t& b)
{
	return boolexpr::xor_s({ a, b });
}

template<>
bx_t bit_xnor(const bx_t& a, const bx_t& b)
{
	return boolexpr::xnor_s({ a, b });
}

template<>
bx_t bit_impl(const bx_t& a, const bx_t& b)
{
	return boolexpr::impl_s(a, b);
}

template<>
bx_t bit_nimpl(const bx_t& a, const bx_t& b)
{
	return boolexpr::nimpl_s(a, b);
}

template<>
bx_t bit_ite(const bx_t& s, const bx_t& d1, const bx_t& d0)
{
	return boolexpr::ite_s(s, d1, d0);
}

template<>
bx_t bit_nite(const bx_t& s, const bx_t& d1, const bx_t& d0)
{
	return boolexpr::nite_s(s, d1, d0);
}

template<>
bx_t bit_zero()
{
	return boolexpr::zero()->shared_from_this();
}

template<>
bx_t bit_one()
{
	return boolexpr::one()->shared_from_this();
}
