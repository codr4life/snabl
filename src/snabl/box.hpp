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
		ValueT as() const;

		const ATypePtr &type() const;
		
		void dump(std::ostream &out) const;
	private:
		ATypePtr _type;
		std::any _value;
	};

	template <typename ValueT>
	Box::Box(const TypePtr<ValueT> &type, const ValueT &value):
		_type(type), _value(value) { }

	template <typename ValueT>
	ValueT Box::as() const { return std::any_cast<ValueT>(_value); }
}

#endif
