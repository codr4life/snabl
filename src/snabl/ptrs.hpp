#ifndef SNABL_PTRS_HPP
#define SNABL_PTRS_HPP

#include <memory>

namespace snabl {
	class AType;
	using ATypePtr = std::shared_ptr<AType>;

	template <typename ValueT>
	class Type;

	template <typename ValueT>
	using TypePtr = std::shared_ptr<Type<ValueT>>;

	class Trait;
	using TraitPtr = std::shared_ptr<Trait>;
	
	class Scope;	
	using ScopePtr = std::shared_ptr<Scope>;

	class Macro;
	using MacroPtr = std::shared_ptr<Macro>;
	
	class Fimp;
	using FimpPtr = std::shared_ptr<Fimp>;

	class Func;
	using FuncPtr = std::shared_ptr<Func>;
}

#endif
