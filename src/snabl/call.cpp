#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/call.hpp"

namespace snabl {
	Call::Call(Scope &scope, const FimpPtr &fimp, optional<PC> return_pc):
		scope(scope), fimp(fimp), return_pc(return_pc) { }
	
	Call::Call(Scope &scope, PC return_pc):
		scope(scope), return_pc(return_pc) { }
}
