#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/fimp.hpp"
#include "snabl/lambda.hpp"
#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/state.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"

namespace snabl {	
	class Call {
	public:
		Scope &scope;
		const Pos pos;
		
		const TargetPtr target;
		const optional<const Ops::iterator> return_pc;

		Call(Scope &scope, Pos pos,
				 const FimpPtr &fimp,
				 optional<Ops::iterator> return_pc=nullopt):
			scope(scope), pos(pos), target(fimp), return_pc(return_pc) {
			if (fimp->opts() & Target::Opts::Recalls) { _state.emplace(scope.env); }
		}
	
		Call(Scope &scope, Pos pos, const LambdaPtr &lambda, Ops::iterator return_pc):
			scope(scope), pos(pos), target(lambda), return_pc(return_pc) {
			if (lambda->opts() & Target::Opts::Recalls) { _state.emplace(scope.env); }
		}

		void recall() const;
	private:
    optional<const State> _state;
	};
}

#endif
