#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

#include "snabl/std.hpp"

namespace snabl {
	class Target {
	public:
		virtual void dump(ostream &out) const=0;
	};

	using TargetPtr = shared_ptr<Target>;
}

#endif
