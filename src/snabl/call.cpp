#include "snabl/call.hpp"

namespace snabl {
	Call::Call(const TargetPtr &target, const ScopePtr &scope):
		target(target), scope(scope) { }
}
