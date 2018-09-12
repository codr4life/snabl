#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

namespace snabl {
	class Target {
	public:
		virtual size_t start_pc() const=0;
	};
}

#endif
