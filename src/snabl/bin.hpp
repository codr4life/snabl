#ifndef SNABL_BIN_HPP
#define SNABL_BIN_HPP

#include <deque>
#include <map>

#include "snabl/fimp.hpp"
#include "snabl/op.hpp"
#include "snabl/std/optional.hpp"

namespace snabl {
	class Env;

	using Ops = std::deque<Op>;
	
	class Bin {
	public:
		Env &env;
		Bin(Env &env);
		std::optional<Ops::iterator> get_fimp_offs(const AFimpPtr &ptr);

		Op &emit_op(const OpType &type);
		Op &emit_begin(const ScopePtr &parent);
		Op &emit_end();
		Op &emit_push(const Box &value);

		void run(std::optional<Ops::iterator> begin=std::nullopt,
						 std::optional<Ops::iterator> end=std::nullopt);
	private:
		std::map<AFimpPtr, Ops::iterator> _fimp_offs;
		Ops _ops;
		Ops::iterator _pc;
	};
}

#endif
