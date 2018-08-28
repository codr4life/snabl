#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/std/any.hpp"

namespace snabl {
	class Box {
	public:
		Box(const ATypePtr &type);

		template <typename ValueT>
		Box(const TypePtr<ValueT> &type, const ValueT &value);

		template <typename ValueT>
		ValueT as() const;

		const ATypePtr &type() const;

		bool is_undef() const;

		bool is_equid(const Box &rhs) const;
		bool is_eqval(const Box &rhs) const;
		Cmp cmp(const Box &rhs) const;
		void dump(ostream &out) const;
		void write(ostream &out) const;
	private:
		ATypePtr _type;
		any _value;
	  bool _is_undef;
	};

	template <typename ValueT>
	Box::Box(const TypePtr<ValueT> &type, const ValueT &value):
	  _type(type), _value(value), _is_undef(false) { }

	template <typename ValueT>
	ValueT Box::as() const {
		if (_is_undef) { throw Error("Deref of undef value"); }
		return any_cast<ValueT>(_value);
	}
}

#endif
