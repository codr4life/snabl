#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include <iostream>

#include <array>
#include <cmath>
#include <memory>
#include <sstream>

#include "snabl/bin.hpp"
#include "snabl/form.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/std.hpp"
#include "snabl/stack.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call;

	class Fimp: public Target {
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

		static Sym get_id(const FuncPtr &func, const Args &args);
		static bool call(const FimpPtr &fimp, Pos pos);

		Fimp(const FuncPtr &func, const Args &args, const Rets &rets, Imp imp);
		
		Fimp(const FuncPtr &func,
				 const Args &args, const Rets &rets,
				 Forms::const_iterator begin,
				 Forms::const_iterator end);

		bool compile(Pos pos);
		optional<size_t> score(const Stack &stack) const;
		void dump(ostream &out) const override;
	private:
		optional<PC> _start_pc;
		size_t _nops;
	};
}

#endif
