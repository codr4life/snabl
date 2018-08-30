#include "snabl/call.hpp"

namespace snabl {
	Call::Call(const any &target,
						 const ScopePtr &scope,
						 optional<Ops::iterator> return_pc):
		scope(scope), return_pc(return_pc), _target(target) { }
}
