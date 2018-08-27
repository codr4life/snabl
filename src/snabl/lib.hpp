#ifndef SNABL_LIB_HPP
#define SNABL_LIB_HPP

#include <unordered_map>

#include "snabl/error.hpp"
#include "snabl/func.hpp"
#include "snabl/macro.hpp"
#include "snabl/sym.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class Env;
	
	class Lib {
	public:
		Env &env;
		const Sym id;
		
		Lib(Env &env, Sym id);
		~Lib();
		
		MacroPtr add_macro(Sym id, const Macro::Imp &imp);

		template <typename TypeT, typename... ArgsT>
		std::shared_ptr<TypeT> add_type(Sym id,
																		std::initializer_list<ATypePtr> parent_types={},
																		ArgsT &&... args);

		FuncPtr add_func(Sym id, std::size_t nargs, std::size_t nrets);

		template <typename... ImpT>
		FimpPtr add_fimp(Sym id,
										 const Fimp::Args &args,
										 const Fimp::Rets &rets,
										 ImpT &&... imp);

		MacroPtr get_macro(Sym id);
		ATypePtr get_type(Sym id);
		FuncPtr get_func(Sym id);
	private:
		std::unordered_map<Sym, MacroPtr> _macros;
		std::unordered_map<Sym, ATypePtr> _types;
		std::unordered_map<Sym, FuncPtr> _funcs;
	};

	template <typename TypeT, typename... ArgsT>
	std::shared_ptr<TypeT> Lib::add_type(Sym id,
																			 std::initializer_list<ATypePtr> parent_types,
																			 ArgsT &&... args) {
		auto t(std::make_shared<TypeT>(*this,
																	 id,
																	 std::forward<ArgsT>(args)...));
		_types.emplace(t->id, t);
		for (auto &pt: parent_types) { AType::derive(t, pt); }
		return t;
	}

	template <typename... ImpT>
	FimpPtr Lib::add_fimp(Sym id,
												const Fimp::Args &args,
												const Fimp::Rets &rets,
												ImpT &&... imp) {
		auto fn(add_func(id, args.size(), rets.size()));
		return Func::add_fimp(fn, args, rets, imp...);
	}
}

#endif
