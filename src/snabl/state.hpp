#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

namespace snabl {
	class Env;
	
	struct State {
		const size_t nlibs, nscopes, ncalls, nstack;
		
		State(const Env &env);
		void restore_libs(Env &env) const;
		void restore_scopes(Env &env) const;
		void restore_calls(Env &env) const;
		void restore_stack(Env &env) const;
	};
}

#endif
