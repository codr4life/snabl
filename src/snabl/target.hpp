#ifndef SNABL_TARGET_HPP
#define SNABL_TARGET_HPP

namespace snabl {
	class Target {
	public:
		enum class Opt {Vars, Recalls};
		using Opts = set<Opt>;

		bool opt(Opt o) {
			const auto &os(opts());
			return os.find(o) != os.end();
		}
			
		virtual const Opts &opts() const=0;
		virtual size_t start_pc() const=0;
	};
}

#endif
