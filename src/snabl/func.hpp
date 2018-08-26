#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>
#include <unordered_map>

#include "snabl/box.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std/optional.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Lib;

	class AFunc {
	public:
		Lib &lib;
		const Sym id;
		const int nargs, nrets;
		
		virtual AFimpPtr get_fimp() const=0;
		virtual AFimpPtr get_best_fimp(const Stack &stack) const=0;
		virtual void clear()=0;
	protected:
		AFunc(Lib &lib, Sym id, int nargs, int nrets);
	};

	template <int NARGS, int NRETS>
	class Func: public AFunc {
	public:
		using Args = std::array<Box, NARGS>;
		using Rets = std::array<Box, NRETS>;
		
		template <typename... ImpT>
		static FimpPtr<NARGS, NRETS> add_fimp(const FuncPtr<NARGS, NRETS> &func,
																					const Args &args,
																					const Rets &rets,
																					ImpT &&... imp);
		
		Func(Lib &lib, Sym id);
		AFimpPtr get_fimp() const override;
		AFimpPtr get_best_fimp(const Stack &stack) const override;
		void clear() override;
	private:
		std::unordered_map<Sym, FimpPtr<NARGS, NRETS>> _fimps;
	};

	template <int NARGS, int NRETS>
	Func<NARGS, NRETS>::Func(Lib &lib, Sym id): AFunc(lib, id, NARGS, NRETS) { }

	template <int NARGS, int NRETS>
	template <typename... ImpT>
	FimpPtr<NARGS, NRETS>
	Func<NARGS, NRETS>::add_fimp(const FuncPtr<NARGS, NRETS> &func,
															 const typename Func<NARGS, NRETS>::Args &args,
															 const typename Func<NARGS, NRETS>::Rets &rets,
															 ImpT &&... imp) {
		auto id(Fimp<NARGS, NRETS>::get_id(func, args));
		auto found = func->_fimps.find(id);
		if (found != func->_fimps.end()) { func->_fimps.erase(found); }

		auto f(std::make_shared<Fimp<NARGS, NRETS>>(func,
																								args, rets,
																								std::forward<ImpT>(imp)...));
		func->_fimps.emplace(id, f);
		return f;
	}

	template <int NARGS, int NRETS>
	AFimpPtr Func<NARGS, NRETS>::get_fimp() const {
		if (_fimps.size() != 1) { throw Error("Too many fimps"); }
		return _fimps.begin()->second;
	}
	
	template <int NARGS, int NRETS>
	AFimpPtr Func<NARGS, NRETS>::get_best_fimp(const Stack &stack) const {
		std::optional<std::size_t> best_score;
		AFimpPtr best_fimp;
		
		for (auto &fp: _fimps) {
			auto &f(fp.second);
			auto fs(f->score(stack));
			
			if (fs) {
				if (*fs == 0) { return f; }
			
				if (!best_score || fs < best_score) {
					best_score = fs;
					best_fimp = f;
				}
			}
		}

		if (!best_fimp) { throw Error("Func not applicable: " + id.name()); }
		return best_fimp;
	}

	template <int NARGS, int NRETS>
	void Func<NARGS, NRETS>::clear() { _fimps.clear(); }
}

#endif
