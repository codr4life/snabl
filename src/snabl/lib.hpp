#ifndef SNABL_LIB_HPP
#define SNABL_LIB_HPP

#include <unordered_map>

#include "snabl/error.hpp"
#include "snabl/func.hpp"
#include "snabl/sym.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class Env;
	
	class Lib {
	public:
		Env &env;
		const Sym id;
		
		Lib(Env &env, const Sym &id);

		template <typename TypeT, typename... ArgsT>
		std::shared_ptr<TypeT> add_type(ArgsT &&... args);

		template <int NARGS, int NRETS>
		FuncPtr<NARGS, NRETS> add_func(const Sym &id);

		template <int NARGS, int NRETS, typename... ImpT>
		FimpPtr<NARGS, NRETS> add_fimp(const Sym &id,
																	 const typename Func<NARGS, NRETS>::Args &args,
																	 const typename Func<NARGS, NRETS>::Rets &rets,
																	 ImpT &&... imp);

		AFuncPtr get_func(const Sym &id);
	private:
		std::unordered_map<Sym, ATypePtr> _types;
		std::unordered_map<Sym, AFuncPtr> _funcs;
	};

	template <typename TypeT, typename... ArgsT>
	std::shared_ptr<TypeT> Lib::add_type(ArgsT &&... args) {
		auto t(std::make_shared<TypeT>(*this, std::forward<ArgsT>(args)...));
		_types.emplace(t->id, t);
		return t;
	}

	template <int NARGS, int NRETS>
	FuncPtr<NARGS, NRETS> Lib::add_func(const Sym &id) {
		auto found(_funcs.find(id));
		
		if (found != _funcs.end()) {
			auto f(found->second);

			if (f->nargs != NARGS || f->nrets != NRETS) {
				throw Error("Wrong nr of args");
			}
			
			return std::static_pointer_cast<Func<NARGS, NRETS>>(f);
		}
		
		auto f(std::make_shared<Func<NARGS, NRETS>>(*this, id));
		_funcs.emplace(f->id, f);
		return f;
	}

	template <int NARGS, int NRETS, typename... ImpT>
	FimpPtr<NARGS, NRETS> Lib::add_fimp(const Sym &id,
																			const typename Func<NARGS, NRETS>::Args &args,
																			const typename Func<NARGS, NRETS>::Rets &rets,
																			ImpT &&... imp) {
		auto f(add_func<NARGS, NRETS>(id));
		return Func<NARGS, NRETS>::add_fimp(f, args, rets, imp...);
	}
}

#endif
