#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

#include "snabl/ptrs.hpp"

namespace snabl {
	class Env;
	class Lib;
	
	struct State {
		Lib &lib;
		const TargetPtr target;
		const size_t nscopes, nstack, nsplits;
		
		State(const Env &env);
		void restore_all(Env &env) const;
		void restore_lib(Env &env) const;
		void restore_scopes(Env &env) const;
		void restore_target(Env &env) const;
		void restore_stack(Env &env) const;
		void restore_splits(Env &env) const;
	};
}

#endif
