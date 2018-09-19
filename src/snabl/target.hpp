#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"

namespace snabl {
	class Target {
	public:
		enum class Opts: int {None=0, Vars=1, Recalls=2};

		Target(const ScopePtr &parent_scope=nullptr,
					 const OpImp start_pc=nullptr,
					 const OpImp end_pc=nullptr,
					 Opts opts=Opts::None):
			_parent_scope(parent_scope),
			_start_pc(start_pc), _end_pc(end_pc),
			_opts(opts) { }
		virtual ~Target() { }
		void begin_call(Env &env);
		void recall(Env &env) const;
	protected:
		ScopePtr _parent_scope;
		OpImp _start_pc, _end_pc;
		Opts _opts;
	private:
		optional<const State> _prev_state;
	};

	inline Target::Opts& operator |=(Target::Opts& lhs, Target::Opts rhs) {
    lhs = (Target::Opts)(static_cast<int>(lhs) | static_cast<int>(rhs));
    return lhs;
	}

	inline bool operator &(Target::Opts lhs, Target::Opts rhs) {
    return static_cast<int>(lhs) & static_cast<int>(rhs);
	}
}

#endif
