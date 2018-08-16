#include "snabl/env.hpp"

namespace snabl {
	Env::Env() {
	}

	Sym Env::get_sym(const std::string &name) {
		auto found = _syms.find(name);

		return Sym((found == _syms.end())
							 ? _syms.emplace(name, std::make_shared<SymImp>(name)).first->second
							 : found->second);
	}
}
