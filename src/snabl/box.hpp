#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/atype.hpp"
#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/var.hpp"

namespace snabl {
	class Box {
	public:
		Box(const Box &src): _type(src._type) {
			if (src._val) { _val.emplace(_type->copy(*src._val)); }
		}
		
		Box(const ATypePtr &type): _type(type) { assert(type->size <= AType::MaxSize); }
		
		template <typename ValT>
		Box(const TypePtr<ValT> &type, const ValT &val): _type(type), _val(val) { }

		~Box() {
			if (_val) { _type->destroy(*_val); }
		}
		
		Box &operator=(const Box &src) {
			_type = src._type;

			if (_val) {
				_type->destroy(*_val);
				_val.reset();
			}

			if (src._val) { _val.emplace(_type->copy(*src._val)); }
			return *this;
		}
				
		template <typename ValT>
		const ValT &as() const {
			assert(sizeof(ValT) == _type->size);
			return _val->as<ValT>();
		}

		template <typename ValT>
		ValT &as() {
			assert(sizeof(ValT) == _type->size);
			return _val->as<ValT>();
		}

		const ATypePtr &type() const { return _type; }		
		bool isa(const ATypePtr &rhs) const;
		
		bool equid(const Box &rhs) const;
		bool eqval(const Box &rhs) const;
		Cmp cmp(const Box &rhs) const;

		bool is_true() const;
		bool is_undef() const { return !_val; }

		void call(Pos pos, bool now) const;
		void dump(ostream &out) const;
		void print(ostream &out) const;
		void write(ostream &out) const;
	private:
		ATypePtr _type;
		optional<Var<AType::MaxSize>> _val;
	};

	inline ostream &operator <<(ostream &out, const Box &x) {
		x.print(out);
		return out;
	}
}

#endif
