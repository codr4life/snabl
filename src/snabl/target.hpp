#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

#include "snabl/call.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/types.hpp"

namespace snabl {
	class Target {
	public:
		enum class Opts: int {None=0, Recalls, Regs, Vars};
		
		Target(const ScopePtr &parent_scope=nullptr,
					 const OpImp &start_pc=nullptr, Int end_pc=-1,
					 Opts opts=Opts::None):
			_parent_scope(parent_scope),
			_start_pc(start_pc), _end_pc(end_pc),
			_opts(opts) { }

		virtual ~Target() { }
		virtual string target_id() const=0;
		const OpImp &start_pc() const { return _start_pc; }
		Opts opts() const { return _opts; }
	protected:
		ScopePtr _parent_scope;
		OpImp _start_pc;
		Int _end_pc;
		Opts _opts;

		friend Env;
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
