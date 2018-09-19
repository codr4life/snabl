#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

#include "snabl/ptrs.hpp"

namespace snabl {
	class Env;
	
	struct State {
		const TargetPtr target;
		const size_t nlibs, nscopes, nstack, nsplits;
		
		State(const Env &env);
		void restore_all(Env &env) const;
		void restore_libs(Env &env) const;
		void restore_scopes(Env &env) const;
		void restore_target(Env &env) const;
		void restore_stack(Env &env) const;
		void restore_splits(Env &env) const;
	};
}

#endif
