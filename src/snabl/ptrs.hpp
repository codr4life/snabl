#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include "snabl/ptr.hpp"
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
	using ScopePtr = Ptr<Scope>;

	class Macro;
	using MacroPtr = Ptr<Macro>;

	class Fimp;
	using FimpPtr = Ptr<Fimp>;

	class Func;
	using FuncPtr = Ptr<Func>;

	class Lambda;
	using LambdaPtr = Ptr<Lambda>;

	class UserError;
	using ErrorPtr = Ptr<UserError>;
}

#endif
