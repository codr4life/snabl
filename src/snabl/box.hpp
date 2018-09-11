#ifndef SNABL_BOX_HPP
#define SNABL_BOX_HPP

#include "snabl/atype.hpp"
#include "snabl/cmp.hpp"
#include "snabl/error.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Box {
	public:
		static const size_t MaxSize = 16;
		
		Box(const ATypePtr &type): _type(type), _val(), _is_undef(true) {
			assert(type->size <= MaxSize);
		}
		
		template <typename ValT>
		Box(const TypePtr<ValT> &type, const ValT &val): _type(type), _is_undef(false) {
			assert(type->size <= MaxSize);
			new(_val.begin()) ValT(val);
		}

		template <typename ValT>
		const ValT &as() const {
			assert(sizeof(ValT) == _type->size);
			if (_is_undef) { throw Error("Deref of undef val"); }
			return *reinterpret_cast<const ValT *>(_val.begin());
		}

		template <typename ValT>
		ValT &as() {
			assert(sizeof(ValT) == _type->size);
			if (_is_undef) { throw Error("Deref of undef val"); }
			return *reinterpret_cast<ValT *>(_val.begin());
		}

		const ATypePtr &type() const { return _type; }
		
		bool isa(const ATypePtr &rhs) const;
		bool equid(const Box &rhs) const;
		bool eqval(const Box &rhs) const;
		Cmp cmp(const Box &rhs) const;
		bool is_true() const;
		bool is_undef() const { return _is_undef; }
		void call(Pos pos, bool now) const;
		void dump(ostream &out) const;
		void print(ostream &out) const;
		void write(ostream &out) const;
	private:
		ATypePtr _type;
		array<unsigned char, MaxSize> _val;
		bool _is_undef;
	};

	inline ostream &operator <<(ostream &out, const Box &x) {
		x.print(out);
		return out;
	}
}

#endif
