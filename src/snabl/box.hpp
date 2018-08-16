#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/std/any.hpp"
#include "snabl/type.hpp"

namespace snabl {
	struct Box {
	public:
		template <typename ValueT>
		Box(const TypePtr<ValueT> &type, const ValueT &value);
	private:
		ATypePtr _type;
		std::any _value;
	};

	template <typename ValueT>
	Box::Box(const TypePtr<ValueT> &type, const ValueT &value):
		_type(type), _value(value) {
	}
}

#endif
