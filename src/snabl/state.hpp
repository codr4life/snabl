#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

namespace snabl {
	class Env;
	
	struct State {
		const size_t nlibs, nscopes, nstacks;
		
		State(const Env &env);
		void restore(Env &env) const;
	};
}

#endif
