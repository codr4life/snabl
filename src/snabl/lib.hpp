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

		template <int NARGS, int NRETS>
		AFuncPtr add_func(const Sym &id);
	private:
		std::unordered_map<Sym, ATypePtr> _types;
		std::unordered_map<Sym, AFuncPtr> _funcs;
	};

	template <typename TypeT, typename... ArgsT>
	std::shared_ptr<TypeT> Lib::add_type(ArgsT&&... args) {
		auto t(std::make_shared<TypeT>(args...));
		_types.emplace(t->id, t);
		return t;
	}

	template <int NARGS, int NRETS>
	AFuncPtr Lib::add_func(const Sym &id) {
		auto found(_funcs.find(id));
		if (found != _funcs.end()) { return found->second; }
		auto f(std::make_shared<Func<NARGS, NRETS>>(id));
		_funcs.emplace(f->id, f);
		return f;
	}
}

#endif
