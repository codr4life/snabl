#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include "snabl/def.hpp"
#include "snabl/form.hpp"
#include "snabl/op.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/stack.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call;

	class Fimp: public Def, public Target {
	public:
		using Args = vector<Box>;
		using Rets = vector<ATypePtr>;					
		using Imp = function<void (Call &)>;
		
		const FuncPtr func;
		const Args args;
		const Rets rets;
		const Forms forms;
		const optional<Imp> imp;

		static Sym get_id(const Func &func, const Args &args);
		static bool compile(const FimpPtr &fimp, Pos pos);
		static void call(const FimpPtr &fimp, Pos pos);

		Fimp(const FuncPtr &func, const Args &args, const Rets &rets, Imp imp);
		
		Fimp(const FuncPtr &func,
				 const Args &args, const Rets &rets,
				 Forms::const_iterator begin,
				 Forms::const_iterator end);

		size_t start_pc() const override { return *_start_pc; }
		size_t nops() const { return _nops; }
		optional<size_t> score(const Stack &stack) const;
	private:
		optional<size_t> _start_pc;
		size_t _nops;
		bool _has_vars;
	};
}

#endif
