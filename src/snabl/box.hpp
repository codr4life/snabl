#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/std/any.hpp"

namespace snabl {
	class AType;
	
	template <typename ValueT>
	class Type;

	struct Box {
	public:
		template <typename ValueT>
		Box(Type<ValueT> &type, const ValueT &value);
	private:
		AType &_type;
		std::any _value;
	};

	template <typename ValueT>
	Box::Box(Type<ValueT> &type, const ValueT &value): _type(type), _value(value) {
	}
}

#endif
