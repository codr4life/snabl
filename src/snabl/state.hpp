#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

#include "snabl/ptrs.hpp"

namespace snabl {
	class Env;
	class Lib;

	namespace ops { struct Try; }
	
	struct State {
		State(const Env &env);
		void restore_lib(Env &env) const;
		void restore_scope(Env &env) const;
		void restore_tries(Env &env) const;
		void restore_calls(Env &env) const;
		void restore_stack(Env &env) const;
		void restore_splits(Env &env) const;
	private:
		Lib &_lib;
		const ScopePtr _scope;
		const Int _ncalls, _ntries, _nstack, _nsplits;
	};
}

#endif
