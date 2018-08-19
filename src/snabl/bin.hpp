#ifndef SNABL_BIN_HPP
#define SNABL_BIN_HPP

#include <deque>
#include <unordered_map>

#include "snabl/fimp.hpp"
#include "snabl/op.hpp"
#include "snabl/std/optional.hpp"

namespace snabl {
	class Env;

	using Ops = std::deque<Op>;

	struct BinFimp {
		const size_t begin, end;
		BinFimp(size_t begin, size_t end);
	};
	
	class Bin {
	public:
		Env &env;
		Bin(Env &env);
		const Ops &ops() const;
		std::optional<BinFimp> get_fimp(const AFimpPtr &ptr);

		template <typename ImpT, typename... ArgsT>
		Op &emplace_back(const OpType<ImpT> &type, ArgsT &&... args);
		
		void compile(const Forms &forms);
		void run(size_t offs=0);
	private:
		std::unordered_map<AFimpPtr, BinFimp> _fimps;
		Ops::iterator _pc;
		Ops _ops;
	};

	template <typename ImpT, typename... ArgsT>
	Op &Bin::emplace_back(const OpType<ImpT> &type, ArgsT &&... args) {
		_ops.emplace_back(type, args...);
		return _ops.back();
	}
}

#endif
