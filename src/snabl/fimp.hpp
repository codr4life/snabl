#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include <iostream>

#include <array>
#include <cmath>
#include <memory>
#include <sstream>

#include "snabl/form.hpp"
#include "snabl/ptrs.hpp"
#include "snabl/stdx/optional.hpp"
#include "snabl/stack.hpp"
#include "snabl/target.hpp"

namespace snabl {
	class Call;

	class Fimp: public Target {
	public:
		using Args = std::vector<Box>;
		using Rets = std::vector<ATypePtr>;					
		using Imp = std::function<void (Call &)>;
		
		const Sym id;

		const FuncPtr func;
		const Args args;
		const Rets rets;
		const Forms forms;

		static Sym get_id(const FuncPtr &func, const Args &args);
		static void call(const FimpPtr &fimp);

		Fimp(const FuncPtr &func, const Args &args, const Rets &rets, Imp imp);
		
		Fimp(const FuncPtr &func,
				 const Args &args, const Rets &rets,
				 Forms::const_iterator begin,
				 Forms::const_iterator end);

		stdx::optional<std::size_t> score(const Stack &stack) const;
		void dump(std::ostream &out) const override;
	private:
		const stdx::optional<Imp> _imp;
	};
}

#endif
