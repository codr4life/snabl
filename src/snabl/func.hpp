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
		const size_t nargs;

		template <typename... ImpT>
		static const FimpPtr &add_fimp(const FuncPtr &func,
																	 const Fimp::Args &args,
																	 ImpT &&... imp);
		
		Func(Lib &lib, Sym id, size_t nargs): Def(id), lib(lib), nargs(nargs) { }

		const FimpPtr &get_fimp() const { return _fimps.begin()->second; }

		const FimpPtr *get_best_fimp(Stack::const_iterator begin,
																 Stack::const_iterator end) const {
			ssize_t best_score(-1);
			FimpPtr *best_fimp(nullptr);
			
			for (auto &fp: _fimps) {
				auto &f(const_cast<FimpPtr &>(fp.second));
				auto fs(f->score(begin, end));
				
				if (fs != -1) {
					if (fs == 0) { return &f; }
					
					if (best_score == -1 || fs < best_score) {
						best_score = fs;
						best_fimp = &f;
					}
				}
			}
			
			return best_fimp;
		}

		void clear() { _fimps.clear(); }
	private:
		unordered_map<Sym, FimpPtr> _fimps;
	};

	template <typename... ImpT>
	const FimpPtr &Func::add_fimp(const FuncPtr &func,
																const Fimp::Args &args,
																ImpT &&... imp) {
		auto id(Fimp::get_id(*func, args));
		auto found = func->_fimps.find(id);
		if (found != func->_fimps.end()) { func->_fimps.erase(found); }

		return func->_fimps.emplace(id,
																make_shared<Fimp>(func, args, forward<ImpT>(imp)...))
			.first->second;
	}
}

#endif
