#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Box {
	public:
		
		Box(const ATypePtr &type): _type(type), _val(nullptr), _is_undef(true) { }
		
		template <typename ValT>
		Box(const TypePtr<ValT> &type, const ValT &val):
			_type(type), _val(val), _is_undef(false) { }

		template <typename ValT>
		ValT as() const {
			if (_is_undef) { throw Error("Deref of undef val"); }
			return any_cast<ValT>(_val);
		}

		const ATypePtr &type() const { return _type; }
		
		bool is_equid(const Box &rhs) const;
		bool is_eqval(const Box &rhs) const;
		Cmp cmp(const Box &rhs) const;
		bool is_true() const;
		bool is_undef() const { return _is_undef; }
		void call(bool now) const;
		void dump(ostream &out) const;
		void print(ostream &out) const;
		void write(ostream &out) const;
	private:
		ATypePtr _type;
		any _val;
		bool _is_undef;
	};

	string fmt_arg(const Box &x);
}

#endif
