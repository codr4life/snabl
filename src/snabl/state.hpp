#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

#include "snabl/ptrs.hpp"

namespace snabl {
	class Env;
	class Lib;

	namespace ops { class Try; }
	
	struct State {
		State(const Env &env);
		void restore_lib(Env &env) const;
		void restore_scope(Env &env) const;
		void restore_try(Env &env) const;
		void restore_calls(Env &env) const;
		void restore_stack(Env &env) const;
		void restore_splits(Env &env) const;
	private:
		Lib &_lib;
		const ScopePtr _scope;
		ops::Try *const _try;
		const size_t _ncalls, _nstack, _nsplits;
	};
}

#endif
