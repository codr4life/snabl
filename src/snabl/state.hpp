#ifndef SNABL_STATE_HPP
#define SNABL_STATE_HPP

#include "snabl/ptrs.hpp"

namespace snabl {
	class Env;
	class Lib;

	namespace ops { class Try; }
	
	class State {
	public:
		State(const Env &env);
		void restore_lib(Env &env) const;
		void restore_target(Env &env) const;
		void restore_try(Env &env) const;
		void restore_scopes(Env &env) const;
		void restore_stack(Env &env) const;
		void restore_splits(Env &env) const;
	private:
		Lib &_lib;
		const TargetPtr _target;
		ops::Try *const _try;
		const size_t _nscopes, _nstack, _nsplits;
	};
}

#endif
