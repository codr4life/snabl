#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include <array>
#include <memory>
#include <sstream>

#include "snabl/func.hpp"
#include "snabl/std/optional.hpp"

namespace snabl {
	class Call;

	class AFimp {
	public:
		using Imp = std::function<void (Call &)>;
		const Sym id;
		const std::optional<Imp> imp;
		
		AFimp(const Sym &id, Imp imp);
	};

	using AFimpPtr = std::shared_ptr<AFimp>;

	template <int NARGS, int NRETS>
	class Fimp: public AFimp {
	public:
		using Args = typename Func<NARGS, NRETS>::Args;
		using Rets = typename Func<NARGS, NRETS>::Rets;
			
		const FuncPtr<NARGS, NRETS> func;
		const Args args;
		const Rets rets;

		static Sym get_id(const FuncPtr<NARGS, NRETS> &func, const Args &args);

		Fimp(const FuncPtr<NARGS, NRETS> &func, 
				 const Args &args, 
				 const Rets &rets, 
				 Imp imp);
	};

	template <int NARGS, int NRETS>
	Sym Fimp<NARGS, NRETS>::get_id(const FuncPtr<NARGS, NRETS> &func, 
																 const Args &args) {
		std::stringstream buf;
		buf << func->id.name() << '<';
		char sep = 0;

		for (auto &a: args) {
			if (sep) {
				buf << sep;
			} else {
				sep = ' ';
			}

			buf << a->id.name();
		}

		buf << '>';
		return func->lib.env.get_sym(buf.str());
	}

	template <int NARGS, int NRETS>
	Fimp<NARGS, NRETS>::Fimp(const FuncPtr<NARGS, NRETS> &func,
													 const Args &args,
													 const Rets &rets,
													 Imp imp):
		AFimp(get_id(func, args), imp), func(func), args(args), rets(rets) {
	}
}

#endif
