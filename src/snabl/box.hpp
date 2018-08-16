#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/std/any.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class Box {
	public:
		template <typename ValueT>
		Box(const TypePtr<ValueT> &type, const ValueT &value);

		template <typename ValueT>
		ValueT value() const;
	private:
		ATypePtr _type;
		std::any _value;
	};

	template <typename ValueT>
	Box::Box(const TypePtr<ValueT> &type, const ValueT &value):
		_type(type), _value(value) {
	}

	template <typename ValueT>
	ValueT Box::value() const { return std::any_cast<ValueT>(_value); }
}

#endif
