#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

#include "snabl/call.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"

namespace snabl {
	class Target {
	public:
		enum class Opts: int {None=0, Vars=1, Recalls=2};

		static void begin_call(const TargetPtr &t,
													 Env &env,
													 Pos pos,
													 PC return_pc=nullptr);

		Target(const ScopePtr &parent_scope=nullptr,
					 const OpImp start_pc=nullptr,
					 const OpImp end_pc=nullptr,
					 Opts opts=Opts::None):
			_parent_scope(parent_scope),
			_start_pc(start_pc), _end_pc(end_pc),
			_opts(opts) { }
		virtual ~Target() { }
		virtual string target_id() const=0;
		Opts opts() const { return _opts; }
		const optional<const Call> &call() const { return _call; }
		void end_call(Env &env);
		void recall(Env &env) const;
	protected:
		ScopePtr _parent_scope;
		OpImp _start_pc, _end_pc;
		Opts _opts;
		optional<const Call> _call;
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
