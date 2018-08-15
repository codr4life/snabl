#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include <experimental/any>

namespace snabl {
	class AType;
	
	template <typename ValueT>
	class Type;

	using Value = std::experimental::any;
	
	struct Box {
	public:
		template <typename ValueT>
		Box(Type<ValueT> &type, const ValueT &value);
	private:
		AType &_type;
		Value _value;
	};

	template <typename ValueT>
	Box::Box(Type<ValueT> &type, const ValueT &value): _type(type), _value(value) { }
}

#endif
