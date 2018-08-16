#ifndef SNABL_LIB_HPP
#define SNABL_LIB_HPP

#include <unordered_map>

#include "snabl/func.hpp"
#include "snabl/sym.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class Lib {
	public:
		const Sym id;
		Lib(const Sym &id);

		template <typename TypeT, typename... ArgsT>
		std::shared_ptr<TypeT> add_type(ArgsT&&... args);
		FuncPtr add_func(const Sym &id);
	private:
		std::unordered_map<Sym, ATypePtr> _types;
		std::unordered_map<Sym, FuncPtr> _funcs;
	};

	template <typename TypeT, typename... ArgsT>
	std::shared_ptr<TypeT> Lib::add_type(ArgsT&&... args) {
		auto t(std::make_shared<TypeT>(args...));
		_types.emplace(t->id, t);
		return t;
	}
}

#endif
