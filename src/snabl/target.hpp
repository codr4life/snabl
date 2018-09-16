#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

namespace snabl {
	class Target {
	public:
		enum class Opts: int {None=0, Vars=1, Recalls=2};

		virtual ~Target() { }
		virtual Opts opts() const=0;
		virtual size_t start_pc() const=0;
	};

	inline Target::Opts& operator |=(Target::Opts& lhs, Target::Opts rhs) {
    lhs = (Target::Opts)(static_cast<int>(lhs) | static_cast<int>(rhs));
    return lhs;
	}

	inline bool operator &(Target::Opts lhs, Target::Opts rhs) {
    return static_cast<int>(lhs) & static_cast<int>(rhs);
	}
}

#endif
