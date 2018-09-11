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
			if (src._val) { _type->copy(_val, *src._val); }
		}
		
		Box(const ATypePtr &type): _type(type) { }
		
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

			if (src._val) { _type->copy(_val, *src._val); }
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
		
		bool equid(const Box &rhs) const {
			if (rhs.type() != _type) { return false; }
			return _type->equid(*this, rhs);
		}
		
		bool eqval(const Box &rhs) const {
			if (rhs.type() != _type) { return false; }
			return _type->eqval(*this, rhs);
		}
		
		Cmp cmp(const Box &rhs) const {
			auto rt(rhs.type());
			if (rt != _type) { return snabl::cmp(_type->tag, rt->tag); }
			return _type->cmp(*this, rhs);
		}

		bool is_true() const { return _type->is_true(*this); }
		bool is_undef() const { return !_val; }

		void call(Pos pos, bool now) const { _type->call(*this, pos, now); }
		void dump(ostream &out) const { _type->dump(*this, out); }
		void print(ostream &out) const { _type->print(*this, out); }
		void write(ostream &out) const { _type->write(*this, out); }
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
