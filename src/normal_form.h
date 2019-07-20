#ifndef DESHA256_NORMAL_FORM_H_
#define DESHA256_NORMAL_FORM_H_

#include <algorithm>
#include <vector>

#include "clause.h"

template <size_t N>
class NormalForm {
private:
	using normal_form_t = NormalForm<N>;
	using clause_t = Clause<N>;
	using clause_set_t = std::vector<clause_t>;

public:
	NormalForm() {}
	NormalForm(const clause_set_t& cnf, const clause_set_t& dnf) : cnf_(cnf), dnf_(dnf) {}
	NormalForm(size_t i) {
		cnf_.emplace_back(i);
		dnf_.emplace_back(i);
	}
	NormalForm(bool b) {
		if (b) {
			cnf_.emplace_back();
			dnf_.emplace_back();
		}
	}

	normal_form_t operator~() const {
		clause_set_t d, c;

		d.reserve(cnf_.size());
		std::transform(cnf_.begin(), cnf_.end(), std::back_inserter(d), [](const clause_t& x) {
			return x.flip();
		});
		d.shrink_to_fit();

		c.reserve(dnf_.size());
		std::transform(dnf_.begin(), dnf_.end(), std::back_inserter(c), [](const clause_t& x) {
			return x.flip();
		});
		c.shrink_to_fit();

		return {c, d};
	}

	normal_form_t operator&(const normal_form_t& other) const {
		return {cat(cnf_, other.cnf_), product(dnf_, other.dnf_)};
	}

	normal_form_t operator|(const normal_form_t& other) const {
		return {product(cnf_, other.cnf_), cat(dnf_, other.dnf_)};
	}

	normal_form_t operator^(const normal_form_t& other) const {
		return (*this & ~other) | (~*this & other);
	}

	const clause_set_t& cnf() const {
		return cnf_;
	}

	const clause_set_t& dnf() const {
		return dnf_;
	}

	~NormalForm() {}

private:
	static clause_set_t absorb(const clause_set_t& a) {
		if (a.size() <= 1) {
			return a;
		}

		std::vector<char> keep(a.size(), '\x01');

		for (size_t i = 0; i < a.size() - 1; i++) {
			if (!keep[i]) {
				continue;
			}

			for (size_t j = i + 1; j < a.size(); j++) {
				if (a[i].includes(a[j])) {
					keep[i] = '\0';
					break;
				}

				if (a[j].includes(a[i])) {
					keep[j] = '\0';
				}
			}
		}

		clause_set_t r;
		r.reserve(a.size());
		size_t i = 0;
		std::copy_if(a.begin(), a.end(), std::back_inserter(r), [&keep, &i](const clause_t& x) {
			return keep[i++];
		});
		r.shrink_to_fit();
		return a;
	}

	static clause_set_t cat(const clause_set_t& a, const clause_set_t& b) {
		clause_set_t r;
		r.reserve(a.size() + b.size());
		r.insert(r.end(), a.begin(), a.end());
		r.insert(r.end(), b.begin(), b.end());
		return absorb(r);
	}

	static clause_set_t product(const clause_set_t& a, const clause_set_t& b) {
		clause_set_t r;
		r.reserve(a.size() * b.size());

		std::vector<bool> keep_vec(a.size() * b.size(), true);

		for (const clause_t& i : a) {
			for (const clause_t& j : b) {
				const clause_t x = i | j;

				if (!x.valid()) {
					continue;
				}

				bool keep = true;

				for (size_t k = 0; k < r.size(); k++) {
					if (!keep_vec[k]) {
						continue;
					}

					if (x.includes(r[k])) {
						keep = false;
						break;
					}

					if (r[k].includes(x)) {
						keep_vec[k] = false;
					}
				}

				if (keep) {
					r.push_back(x);
				}
			}
		}

		size_t i = 0;
		r.erase(std::remove_if(r.begin(), r.end(), [&i, &keep_vec](const clause_t& x) {
			return !keep_vec[i++];
		}), r.end());

		r.shrink_to_fit();

		return r;
	}

private:
	clause_set_t cnf_, dnf_;
};

#endif  // !DESHA256_NORMAL_FORM_H_
