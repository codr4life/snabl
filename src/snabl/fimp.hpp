#ifndef SNABL_FIMP_HPP
#define SNABL_FIMP_HPP

#include <iostream>

#include <array>
#include <cmath>
#include <memory>
#include <sstream>

#include "snabl/form.hpp"
#include "snabl/func.hpp"
#include "snabl/std/optional.hpp"
#include "snabl/target.hpp"
#include "snabl/type.hpp"

namespace snabl {
	class Call;

	class AFimp: public Target {
	public:
		using Imp = std::function<void (Call &)>;
		const Sym id;
		const Forms forms;
		
		static void call(const AFimpPtr &fimp);
		virtual AFuncPtr afunc() const=0;
		virtual ssize_t score(const Stack &stack) const=0;
		virtual void dump(std::ostream &out) const;
	protected:
		AFimp(const Sym &id, Imp imp);
		AFimp(const Sym &id, Forms &&forms);
	private:
		const std::optional<Imp> _imp;
	};

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

		Fimp(const FuncPtr<NARGS, NRETS> &func, 
				 const Args &args, 
				 const Rets &rets, 
				 Forms &&forms);

		AFuncPtr afunc() const override;
		ssize_t score(const Stack &stack) const override;
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

			if (a.is_undef()) {
				buf << a.type()->id.name();
			} else {
				a.write(buf);
			}
		}

		buf << '>';
		return func->lib.env.get_sym(buf.str());
	}

	template <int NARGS, int NRETS>
	Fimp<NARGS, NRETS>::Fimp(const FuncPtr<NARGS, NRETS> &func,
													 const Args &args,
													 const Rets &rets,
													 Imp imp):
		AFimp(get_id(func, args), imp), func(func), args(args), rets(rets) { }

	template <int NARGS, int NRETS>
	Fimp<NARGS, NRETS>::Fimp(const FuncPtr<NARGS, NRETS> &func,
													 const Args &args,
													 const Rets &rets,
													 Forms &&forms):
		AFimp(get_id(func, args), std::move(forms)), func(func), args(args),
		rets(rets) { }

	template <int NARGS, int NRETS>
	AFuncPtr Fimp<NARGS, NRETS>::afunc() const { return func; }

	template <int NARGS, int NRETS>
	ssize_t Fimp<NARGS, NRETS>::score(const Stack &stack) const {
		if (!NARGS) { return 0; }
		if (stack.size() < NARGS) { return -1; }
		auto &env(func->lib.env);
		auto i(std::next(stack.begin(), stack.size()-NARGS));
		auto j(args.begin());
		size_t score(0);
		
		for (; j != args.end(); i++, j++) {
			if (i == stack.end()) { return -1; }
			auto &iv(*i), &jv(*j);
			auto it(iv.type()), jt(jv.type());
			if (it == env.no_type) { continue; }

			if (jv.is_undef()) {
				if (!it->isa(jt)) { return -1; }
			} else {
				if (!iv.is_eqval(jv)) { return -1; }
			}
			
			score += std::abs(it->tag-jt->tag);
		}

		return score;
	}
}

#endif
