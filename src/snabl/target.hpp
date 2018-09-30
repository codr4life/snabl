#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

#include "snabl/call.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/types.hpp"

namespace snabl {
	class Target {
	public:
		Target(const ScopePtr &parent_scope=nullptr,
					 const OpImp &start_pc=nullptr, Int end_pc=-1):
			_parent_scope(parent_scope),
			_start_pc(start_pc), _end_pc(end_pc) { }

		virtual ~Target() { }
		virtual string target_id() const=0;
		const OpImp &start_pc() const { return _start_pc; }
	protected:
		ScopePtr _parent_scope;
		OpImp _start_pc;
		Int _end_pc;

		friend Env;
	};
}

#endif
