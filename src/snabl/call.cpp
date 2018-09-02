#include "snabl/fimp.hpp"
#include "snabl/func.hpp"
#include "snabl/call.hpp"

namespace snabl {
	Call::Call(Scope &scope, Fimp &fimp, optional<PC> return_pc):
		scope(scope), fimp(&fimp), return_pc(return_pc) { }
	
	Call::Call(Scope &scope, PC return_pc):
		scope(scope), fimp(nullptr), return_pc(return_pc) { }
}
