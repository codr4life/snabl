#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>
#include <unordered_map>

#include "snabl/sym.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class Lib;
	
	class AFunc {
	public:
		Lib &lib;
		const Sym id;
		const int nargs, nrets;
		
		AFunc(Lib &lib, const Sym &id, int nargs, int nrets);
	};

	using AFuncPtr = std::shared_ptr<AFunc>;

	template <int NARGS, int NRETS> 
	class Func;
	
	template <int NARGS, int NRETS> 
	using FuncPtr = std::shared_ptr<Func<NARGS, NRETS>>;

	template <int NARGS, int NRETS>
	class Fimp;
	
	template <int NARGS, int NRETS>
	using FimpPtr = std::shared_ptr<Fimp<NARGS, NRETS>>;
	
	template <int NARGS, int NRETS>
	class Func: public AFunc {
	public:
		using Args = std::array<ATypePtr, NARGS>;
		using Rets = std::array<ATypePtr, NRETS>;
		
		template <typename... ImpT>
		static FimpPtr<NARGS, NRETS> add_fimp(const FuncPtr<NARGS, NRETS> &func,
																					const Args &args,
																					const Rets &rets,
																					ImpT... imp);
		
		Func(Lib &lib, const Sym &id);
	private:
		std::unordered_map<Sym, FimpPtr<NARGS, NRETS>> _fimps;
	};

	template <int NARGS, int NRETS>
	Func<NARGS, NRETS>::Func(Lib &lib, const Sym &id): AFunc(lib, id, NARGS, NRETS) { }

	template <int NARGS, int NRETS>
	template <typename... ImpT>
	FimpPtr<NARGS, NRETS>
	Func<NARGS, NRETS>::add_fimp(const FuncPtr<NARGS, NRETS> &func,
															 const typename Func<NARGS, NRETS>::Args &args,
															 const typename Func<NARGS, NRETS>::Rets &rets,
															 ImpT... imp) {
		auto id(Fimp<NARGS, NRETS>::get_id(func, args));
		auto found = func->_fimps.find(id);
		if (found != func->_fimps.end()) { func->_fimps.erase(found); }

		auto f(std::make_shared<Fimp<NARGS, NRETS>>(func, args, rets, imp...));
		func->_fimps.emplace(id, f);
		return f;
	}
}


#endif
