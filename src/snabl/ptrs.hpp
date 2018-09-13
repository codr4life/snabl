#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include "snabl/std.hpp"

namespace snabl {
	class AType;
	using ATypePtr = shared_ptr<AType>;

	template <typename ValueT>
	class Type;

	template <typename ValueT>
	using TypePtr = shared_ptr<Type<ValueT>>;

	class Trait;
	using TraitPtr = shared_ptr<Trait>;
	
	class Scope;	
	using ScopePtr = shared_ptr<Scope>;

	class Macro;
	using MacroPtr = shared_ptr<Macro>;

	class Target;
	using TargetPtr = shared_ptr<Target>;
	
	class Fimp;
	using FimpPtr = shared_ptr<Fimp>;

	class Func;
	using FuncPtr = shared_ptr<Func>;

	class Lambda;
	using LambdaPtr = shared_ptr<Lambda>;

	class Iter;
	using IterPtr = shared_ptr<Iter>;

	class UserError;
	using ErrorPtr = shared_ptr<UserError>;
}

#endif
