#pragma once

#include <boolexpr/boolexpr.h>

#include "word.h"

template<> boolexpr::bx_t bit_and(const boolexpr::bx_t& a, const boolexpr::bx_t& b) { return boolexpr::and_s({ a, b }); }
template<> boolexpr::bx_t bit_or(const boolexpr::bx_t& a, const boolexpr::bx_t& b) { return boolexpr::or_s({ a, b }); }
template<> boolexpr::bx_t bit_xor(const boolexpr::bx_t& a, const boolexpr::bx_t& b) { return boolexpr::xor_s({ a, b }); }
template<> boolexpr::bx_t bit_zero() { return boolexpr::zero()->shared_from_this(); }
template<> boolexpr::bx_t bit_one() { return boolexpr::one()->shared_from_this(); }
