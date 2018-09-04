#include "snabl/box.hpp"
#include "snabl/type.hpp"

namespace snabl {
	bool Box::is_equid(const Box &rhs) const {
		if (rhs.type() != _type) { return false; }
		return _type->is_equid(*this, rhs);
	}

	bool Box::is_eqval(const Box &rhs) const {
		if (rhs.type() != _type) { return false; }
		return _type->is_eqval(*this, rhs);
	}

	Cmp Box::cmp(const Box &rhs) const {
		auto rt(rhs.type());
		if (rt != _type) { return snabl::cmp(_type->tag, rt->tag); }
		return _type->cmp(*this, rhs);
	}

	bool Box::is_true() const { return _type->is_true(*this); }

	void Box::call(bool now) const { _type->call(*this, now); }

	void Box::dump(ostream &out) const { _type->dump(*this, out); }

	void Box::print(ostream &out) const { _type->print(*this, out); }

	void Box::write(ostream &out) const { _type->write(*this, out); }

	string fmt_arg(const Box &x) {
		stringstream s;
		x.print(s);
		return s.str();
	}
}
