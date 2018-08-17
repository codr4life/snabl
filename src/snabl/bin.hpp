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
		const Ops::iterator begin, end;
		BinFimp(const Ops::iterator &begin, const Ops::iterator &end);
	};
	
	class Bin {
	public:
		Env &env;
		Bin(Env &env);
		std::optional<BinFimp> get_fimp(const AFimpPtr &ptr);

		template <typename ImpT>
		Op &emit_op(const OpType &type, const ImpT &imp);
		
		Op &emit_begin(const ScopePtr &parent);
		Op &emit_end();
		Op &emit_push(const Box &value);

		void run(std::optional<Ops::iterator> begin=std::nullopt,
						 std::optional<Ops::iterator> end=std::nullopt);
	private:
		std::unordered_map<AFimpPtr, BinFimp> _fimps;
		Ops _ops;
		Ops::iterator _pc;
	};

	template <typename ImpT>
	Op &Bin::emit_op(const OpType &type, const ImpT &imp) {
		_ops.emplace_back(type, imp);
		return _ops.back();
	}
}

#endif
