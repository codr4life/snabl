#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

namespace snabl {
	class Target {
	public:
		virtual void dump(std::ostream &out) const=0;
	};

	using TargetPtr = std::shared_ptr<Target>;
}

#endif
