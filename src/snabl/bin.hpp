#ifndef SNABL_BIN_HPP
#define SNABL_BIN_HPP

#include <deque>
#include <unordered_map>

#include "snabl/op.hpp"
#include "snabl/std.hpp"

namespace snabl {
	class Env;

	using Ops = deque<Op>;
	using PC = Ops::iterator;
	
	class Bin {
	public:
		Env &env;
		Ops ops;
		PC pc;

		Bin(Env &env);

		template <typename ImpT, typename... ArgsT>
		Op &emplace_back(const OpType<ImpT> &type, ArgsT &&... args);

		void compile(const Forms::const_iterator &begin,
								 const Forms::const_iterator &end);
		void compile(const Forms &forms);
		
		void run(size_t start_pc=0);
	};

	template <typename ImpT, typename... ArgsT>
	Op &Bin::emplace_back(const OpType<ImpT> &type, ArgsT &&... args) {
		ops.emplace_back(type, args...);
		return ops.back();
	}
}

#endif
