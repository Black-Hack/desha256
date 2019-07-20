#include <iostream>

#include "boolexpr_util.h"
#include "normal_form.h"
#include "sha256.h"
#include "word.h"

template <size_t N>
std::ostream& operator<<(std::ostream& s, Word<bool, N> x) {
	s << '0' << 'x' << std::hex;
	for (size_t i = 0; i < x.size(); i += 4) {
		bool a = x[i + 0].value(), b = x[i + 1].value(), c = x[i + 2].value(), d = x[i + 3].value();
		s << ((a << 3) | (b << 2) | (c << 1) | (d << 0));
	}
	s << std::dec;
	return s;
}

template <size_t N>
std::ostream& operator<<(std::ostream& s, Word<boolexpr::bx_t, N> x) {
	for (size_t i = 0; i < x.size(); i++) {
		s << x[i].value() << ' ';
	}
	return s;
}

template <size_t N>
std::string clause_to_string(const Clause<N>& set, const std::string& sep) {
	std::string s;
	bool init = false;
	s += '(';
	for (size_t i = 0; i < N; i++) {
		if (set[i].has_value()) {
			if (init) {
				s += sep;
			} else {
				init = true;
			}

			if (!set[i].value()) {
				s += '~';
			}
			s += 'x';
			s += std::to_string(i);
		}
	}
	s += ')';
	return s;
}

template <size_t N>
std::string cluase_set_to_string(const std::vector<Clause<N>>& set, const std::string& sep1, const std::string& sep2) {
	std::string s;
	bool init = false;

	for (size_t i = 0; i < set.size(); i++) {
		if (init) {
			s += sep2;
		} else {
			init = true;
		}

		s += clause_to_string(set[i], sep1);
	}

	return s;
}

template <size_t N>
std::ostream& operator<<(std::ostream& s, const NormalForm<N>& nf) {
	std::cout << "cnf: " << cluase_set_to_string(nf.cnf(), " | ", " & ") << std::endl
			  << "dnf: " << cluase_set_to_string(nf.dnf(), " & ", " | ") << std::endl;
	return s;
}

int main() {
	using T = NormalForm<640>;

	std::cerr << "Start" << std::endl;

	std::unique_ptr<Sha256<T>> sha = std::make_unique<Sha256<T>>();

	for (size_t i = 0; i < 640; i++) {
		sha->Write(T(i));
	}

	const auto& r = sha->Finalize();

	std::cout << r[0].value();

	return 0;
}