#include "snabl/call.hpp"

namespace snabl {
	Call::Call(const TargetPtr &target, const ScopePtr &scope, ssize_t return_pc):
		target(target), scope(scope), return_pc(return_pc) { }
}
