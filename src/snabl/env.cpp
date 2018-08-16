#include "snabl/env.hpp"

namespace snabl {
	Env::Env():
		lobby(get_sym("lobby")),
		int_type(lobby.add_type<IntType>(get_sym("Int"))) {
	}

	Sym Env::get_sym(const std::string &name) {
		auto found = _syms.find(name);

		return Sym((found == _syms.end())
							 ? _syms.emplace(name,
															 std::make_unique<SymImp>(name)).first->second.get()
							 : found->second.get());
	}
}
