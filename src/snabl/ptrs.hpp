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
	using MacroPtr = shared_ptr<Macro>;

	class Fimp;
	using FimpPtr = shared_ptr<Fimp>;

	class Func;
	using FuncPtr = shared_ptr<Func>;
}

#endif
