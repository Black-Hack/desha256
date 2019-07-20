#ifndef DESHA256_CLAUSES_H_
#define DESHA256_CLAUSES_H_

#include <bitset>
#include <optional>

template <size_t N>
class Clause {
private:
	using clause_t = Clause<N>;
	using bitset_t = std::bitset<N>;

public:
	Clause() {}
	Clause(const bitset_t& set, const bitset_t& clear) : set(set), clear(clear) {}
	Clause(size_t i) {
		set[i] = true;
	}

	bool valid() const {
		return (set & clear).none();
	}

	bool includes(const clause_t& x) const {
		return (set | x.set) == set && (clear | x.clear) == clear;
	}

	bool operator==(const clause_t& other) const {
		return set == other.set && clear == other.clear;
	}

	clause_t operator|(const clause_t& other) const {
		return {set | other.set, clear | other.clear};
	}

	std::optional<bool> operator[](size_t i) const {
		if (set[i]) {
			return true;
		}
		if (clear[i]) {
			return false;
		}
		return std::nullopt;
	}

	clause_t flip() const {
		return {clear, set};
	}

	~Clause() {
	}

private:
	bitset_t set, clear;
};

#endif  // !DESHA256_CLAUSES_H_
