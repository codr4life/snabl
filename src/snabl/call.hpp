#ifndef SNABL_CALL_HPP
#define SNABL_CALL_HPP

#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/scope.hpp"

namespace snabl {
	class Call {
	public:
		const ScopePtr scope;
		const optional<PC> return_pc;

		template <typename TargetT>
		TargetT target() const;
		
		Call(const any &target,
				 const ScopePtr &scope,
				 optional<PC> return_pc=nullopt);
	private:
		const any _target;
	};

	template <typename TargetT>
	TargetT Call::target() const {
		return any_cast<TargetT>(_target);
	}

}

#endif
