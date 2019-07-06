#pragma once

#include <array>

template<typename T, size_t N, size_t M>
union nested_array
{
private:
	using flat_t = std::array<T, N>;
	using inner_t = std::array<T, N / M>;
	using nested_t = std::array<inner_t, M>;

public:
	nested_array() : flat() {}
	nested_array(const nested_array<T, N, M>& other) : flat(other.flat) {}

	nested_array(const T* ptr) : flat()
	{
		std::copy_n(ptr, N, flat.begin());
	}

	nested_array(const flat_t& flat) : flat(flat) {}
	nested_array(flat_t&& flat) : flat(flat) {}

	nested_array(const nested_t& nested) : nested(nested) {}
	nested_array(nested_t&& nested) : nested(nested) {}

	nested_array<T, N, M>& operator=(const nested_array<T, N, M>& other) {
		flat = other.flat;
		return *this;
	}

	constexpr const T& operator[](size_t i) const
	{
		return flat[i];
	}
	T& operator[](size_t i)
	{
		return flat[i];
	}

	constexpr const inner_t& operator()(size_t i) const
	{
		return nested[i];
	}
	inner_t& operator()(size_t i)
	{
		return nested[i];
	}

	constexpr const T& operator()(size_t i, size_t j) const
	{
		return nested[i][j];
	}
	T& operator()(size_t i, size_t j)
	{
		return nested[i][j];
	}

	~nested_array() {}

public:
	flat_t flat;
	nested_t nested;
};
