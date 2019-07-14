#ifndef DESHA256_NESTED_CONTAINER_H_
#define DESHA256_NESTED_CONTAINER_H_

#include <array>

template <typename FlatT, typename NestedT>
union NestedContainer {
private:
	using nested_container_t = NestedContainer<FlatT, NestedT>;
	using flat_t = FlatT;
	using nested_t = NestedT;

public:
	NestedContainer() : flat_() {}
	NestedContainer(const nested_container_t& other) : flat_(other.flat_) {}

	NestedContainer(const flat_t& flat) : flat_(flat) {}
	NestedContainer(flat_t&& flat) : flat_(flat) {}

	NestedContainer(const nested_t& nested) : nested_(nested) {}
	NestedContainer(nested_t&& nested) : nested_(nested) {}

	inline nested_container_t& operator=(const nested_container_t& other) {
		flat_ = other.flat_;
		return *this;
	}
	inline nested_container_t& operator=(nested_container_t&& other) {
		flat_ = other.flat_;
		return *this;
	}
	inline nested_container_t& operator=(const flat_t& x) {
		flat_ = x;
		return *this;
	}
	inline nested_container_t& operator=(flat_t&& x) {
		flat_ = x;
		return *this;
	}
	inline nested_container_t& operator=(const nested_t& x) {
		nested_ = x;
		return *this;
	}
	inline nested_container_t& operator=(nested_t&& x) {
		nested_ = x;
		return *this;
	}

	inline constexpr const auto& operator[](size_t i) const {
		return flat_[i];
	}
	inline auto& operator[](size_t i) {
		return flat_[i];
	}

	inline constexpr const auto& operator()(size_t i) const {
		return nested_[i];
	}
	inline auto& operator()(size_t i) {
		return nested_[i];
	}

	inline constexpr const auto& operator()(size_t i, size_t j) const {
		return nested_[i][j];
	}
	inline auto& operator()(size_t i, size_t j) {
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

	~NestedContainer() {}

private:
	flat_t flat_;
	nested_t nested_;
};

#endif  // !DESHA256_NESTED_CONTAINER_H_
