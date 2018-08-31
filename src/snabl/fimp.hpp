#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include "snabl/form.hpp"
#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/stack.hpp"

namespace snabl {
	class Call;

	class Fimp {
	public:
		using Args = vector<Box>;
		using Rets = vector<ATypePtr>;					
		using Imp = function<void (Call &)>;
		
		const Sym id;

		const FuncPtr func;
		const Args args;
		const Rets rets;
		const Forms forms;
		const optional<Imp> imp;

		static Sym get_id(FuncPtr func, const Args &args);
		static bool compile(FimpPtr fimp, Pos pos);
		static void call(FimpPtr fimp, Pos pos);

		Fimp(FuncPtr func, const Args &args, const Rets &rets, Imp imp);
		
		Fimp(FuncPtr func,
				 const Args &args, const Rets &rets,
				 Forms::const_iterator begin,
				 Forms::const_iterator end);

		optional<size_t> score(const Stack &stack) const;
	private:
		optional<PC> _start_pc;
		size_t _nops;
	};
}

#endif
