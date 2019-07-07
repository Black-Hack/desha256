#ifndef DESHA256_NESTED_ARRAY_H_
#define DESHA256_NESTED_ARRAY_H_

#include <array>

template <typename T, size_t N, size_t M>
union NestedArray {
private:
	using flat_t = std::array<T, N>;
	using inner_t = std::array<T, N / M>;
	using nested_t = std::array<inner_t, M>;

public:
	NestedArray() : flat_() {}
	NestedArray(const NestedArray<T, N, M>& other) : flat_(other.flat_) {}

	explicit NestedArray(const T* ptr) : flat_() {
		std::copy_n(ptr, N, flat_.begin());
	}

	explicit NestedArray(const flat_t& flat) : flat_(flat) {}
	explicit NestedArray(flat_t&& flat) : flat_(flat) {}

	explicit NestedArray(const nested_t& nested) : nested_(nested) {}
	explicit NestedArray(nested_t&& nested) : nested_(nested) {}

	inline NestedArray<T, N, M>& operator=(const NestedArray<T, N, M>& other) {
		flat_ = other.flat_;
		return *this;
	}
	inline NestedArray<T, N, M>& operator=(NestedArray<T, N, M>&& other) {
		flat_ = other.flat_;
		return *this;
	}
	inline NestedArray<T, N, M>& operator=(const flat_t& x) {
		flat_ = x;
		return *this;
	}
	inline NestedArray<T, N, M>& operator=(const nested_t& x) {
		nested_ = x;
		return *this;
	}
	inline NestedArray<T, N, M>& operator=(flat_t&& x) {
		flat_ = x;
		return *this;
	}
	inline NestedArray<T, N, M>& operator=(nested_t&& x) {
		nested_ = x;
		return *this;
	}

	inline constexpr const T& operator[](size_t i) const {
		return flat_[i];
	}
	inline T& operator[](size_t i) {
		return flat_[i];
	}

	inline constexpr const inner_t& operator()(size_t i) const {
		return nested_[i];
	}
	inline inner_t& operator()(size_t i) {
		return nested_[i];
	}

	inline constexpr const T& operator()(size_t i, size_t j) const {
		return nested_[i][j];
	}
	inline T& operator()(size_t i, size_t j) {
		return nested_[i][j];
	}

	inline constexpr const flat_t& as_flat() const {
		return flat_;
	}
	inline flat_t& as_flat() {
		return flat_;
	}

	inline constexpr const nested_t& as_nested() const {
		return nested_;
	}
	inline nested_t& as_nested() {
		return nested_;
	}

	~NestedArray() {}

private:
	flat_t flat_;
	nested_t nested_;
};

#endif  // !DESHA256_NESTED_ARRAY_H_
