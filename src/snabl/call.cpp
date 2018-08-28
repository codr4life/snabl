#include "snabl/call.hpp"

namespace snabl {
	Call::Call(const TargetPtr &target,
						 const ScopePtr &scope,
						 optional<Ops::iterator> return_pc):
		target(target), scope(scope), return_pc(return_pc) { }
}
