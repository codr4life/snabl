#ifndef SNABL_BIN_HPP
#define SNABL_BIN_HPP

#include <deque>
#include <unordered_map>

#include "snabl/fimp.hpp"
#include "snabl/op.hpp"
#include "snabl/stdx/optional.hpp"

namespace snabl {
	class Env;

	using Ops = std::deque<Op>;

	class Bin {
	public:
		Env &env;
		Bin(Env &env);

		const Ops &ops() const;
		std::size_t pc() const;
		void set_pc(std::size_t pc);
		
		template <typename ImpT, typename... ArgsT>
		Op &emplace_back(const OpType<ImpT> &type, ArgsT &&... args);

		void compile(const Forms::const_iterator &begin,
								 const Forms::const_iterator &end);
		void compile(const Forms &forms);
		
		void run(std::size_t offs=0);
	private:
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
