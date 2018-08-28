#include "snabl/func.hpp"

namespace snabl {
	Func::Func(Lib &lib, Sym id, size_t nargs, size_t nrets):
		lib(lib), id(id), nargs(nargs), nrets(nrets) { }

	FimpPtr Func::get_fimp() const {
		if (_fimps.size() != 1) { throw Error("Too many fimps"); }
		return _fimps.begin()->second;
	}

	FimpPtr Func::get_best_fimp(const Stack &stack) const {
		optional<size_t> best_score;
		FimpPtr best_fimp;
		
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

	void Func::clear() { _fimps.clear(); }
}
