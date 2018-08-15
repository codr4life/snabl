#include "snabl/env.hpp"

namespace snabl {
	Env::Env() {
	}

	const Sym &Env::get_sym(const std::string &name) {
		auto found = _syms.find(name);

		return (found == _syms.end())
			? _syms.emplace(name, Sym(name)).first->second
			: found->second;
	}
}
