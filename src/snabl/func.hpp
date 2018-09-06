#ifndef SNABL_FUNC_HPP
#define SNABL_FUNC_HPP

#include "snabl/def.hpp"
#include "snabl/fimp.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Lib;

	class Func: public Def {
	public:
		Lib &lib;
		const size_t nargs, nrets;

		template <typename... ImpT>
		static const FimpPtr &add_fimp(const FuncPtr &func,
																	 const Fimp::Args &args, const Fimp::Rets &rets,
																	 ImpT &&... imp);
		
		Func(Lib &lib, Sym id, size_t nargs, size_t nrets):
			Def(id), lib(lib), nargs(nargs), nrets(nrets) { }

		const FimpPtr &get_fimp() {
			return _fimps.begin()->second;
		}

		const FimpPtr &get_best_fimp(const Stack &stack, size_t offs) const {
			optional<size_t> best_score;
			FimpPtr *best_fimp(nullptr);
			
			for (auto &fp: _fimps) {
				auto &f(const_cast<FimpPtr &>(fp.second));
				auto fs(f->score(stack, offs));
				
				if (fs) {
					if (*fs == 0) { return f; }
					
					if (!best_score || fs < best_score) {
						best_score = fs;
						best_fimp = &f;
					}
				}
			}
			
			if (!best_fimp) { throw Error("Func not applicable: " + id.name()); }
			return *best_fimp;
		}

		void clear() { _fimps.clear(); }
	private:
		map<Sym, FimpPtr> _fimps;
	};

	template <typename... ImpT>
	const FimpPtr &Func::add_fimp(const FuncPtr &func,
																const Fimp::Args &args, const Fimp::Rets &rets,
																ImpT &&... imp) {
		auto id(Fimp::get_id(*func, args));
		auto found = func->_fimps.find(id);
		if (found != func->_fimps.end()) { func->_fimps.erase(found); }

		return func->_fimps.emplace(id, FimpPtr::make(func,
																									args, rets,
																									forward<ImpT>(imp)...))
			.first->second;
	}
}

#endif
