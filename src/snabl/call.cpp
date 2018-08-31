#include "snabl/call.hpp"

namespace snabl {
	Call::Call(const ScopePtr &scope, const FimpPtr &fimp, optional<PC> return_pc):
		scope(scope), fimp(fimp), return_pc(return_pc) { }
	
	Call::Call(const ScopePtr &scope, const Lambda &lambda, PC return_pc):
		scope(scope), lambda(lambda), return_pc(return_pc) { }
}
