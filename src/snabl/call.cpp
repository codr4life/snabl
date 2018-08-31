#include "snabl/call.hpp"

namespace snabl {
	Call::Call(const ScopePtr &scope, const FimpPtr &fimp, optional<PC> return_pc):
		scope(scope), fimp(fimp), return_pc(return_pc) { }
	
	Call::Call(const ScopePtr &scope, PC return_pc):
		scope(scope), return_pc(return_pc) { }
}
