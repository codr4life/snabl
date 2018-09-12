#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

namespace snabl {
	class Target {
	public:
		enum class Opt {Vars, Recalls};
		using Opts = set<Opt>;

		virtual const Opts &opts() const=0;
		virtual size_t start_pc() const=0;
	};
}

#endif
