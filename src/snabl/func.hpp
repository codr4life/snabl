#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include <memory>
#include <unordered_map>

#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Lib;

	class AFunc {
	public:
		Lib &lib;
		const Sym id;
		const int nargs, nrets;
		
		virtual AFimpPtr get_best_fimp() const=0;
		virtual void clear()=0;
	protected:
		AFunc(Lib &lib, const Sym &id, int nargs, int nrets);
	};

	template <int NARGS, int NRETS>
	class Func: public AFunc {
	public:
		using Args = std::array<ATypePtr, NARGS>;
		using Rets = std::array<ATypePtr, NRETS>;
		
		template <typename... ImpT>
		static FimpPtr<NARGS, NRETS> add_fimp(const FuncPtr<NARGS, NRETS> &func,
																					const Args &args,
																					const Rets &rets,
																					ImpT &&... imp);
		
		Func(Lib &lib, const Sym &id);
		AFimpPtr get_best_fimp() const override;
		void clear() override;
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
	AFimpPtr Func<NARGS, NRETS>::get_best_fimp() const {
		ssize_t best_score(-1);
		AFimpPtr best_fimp;
		
		for (auto &fp: _fimps) {
			auto &f(fp.second);
			ssize_t fs(f->score());
			if (!fs) { return f; }
			
			if (fs != -1 && (best_score == -1 || fs < best_score)) {
				best_score = fs;
				best_fimp = f;
			}
		}

		if (!best_fimp) { throw Error("Func not applicable"); }
		return best_fimp;
	}

	template <int NARGS, int NRETS>
	void Func<NARGS, NRETS>::clear() { _fimps.clear(); }
}

#endif
