#include "snabl/call.hpp"

namespace snabl {
	Call::Call(const AFimpPtr &fimp, const ScopePtr &scope):
		fimp(fimp), scope(scope) { }
}
