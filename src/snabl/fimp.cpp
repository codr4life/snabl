#include "snabl/env.hpp"
#include "snabl/fimp.hpp"
#include "snabl/lib.hpp"

namespace snabl {
	Sym Fimp::get_id(const FuncPtr &func, const Args &args) {
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

	void Fimp::call(const FimpPtr &fimp) {
		auto &env(fimp->func->lib.env);
						 
		if (fimp->_imp) {
			auto stack_offs(env.stack().size());
			auto &call(env.push_call(fimp, -1));
			(*fimp->_imp)(call);
			auto func(fimp->func);
			
			if (env.stack().size() != stack_offs-func->nargs+func->nrets) {
				throw Error("Invalid stack after funcall");
			}
			
			env.pop_call();
		}
	}

	Fimp::Fimp(const FuncPtr &func, const Args &args, const Rets &rets, Imp imp):
		id(get_id(func, args)), func(func), args(args), rets(rets), _imp(imp) { }

	Fimp::Fimp(const FuncPtr &func,
						 const Args &args, const Rets &rets,
						 Forms::const_iterator begin,
						 Forms::const_iterator end):
		id(get_id(func, args)), func(func), args(args), rets(rets), forms(begin, end) { }

	stdx::optional<std::size_t> Fimp::score(const Stack &stack) const {
		if (!func->nargs) { return 0; }
		if (stack.size() < func->nargs) { return stdx::nullopt; }
		auto &env(func->lib.env);
		auto i(std::next(stack.begin(), stack.size()-func->nargs));
		auto j(args.begin());
		std::size_t score(0);

		for (; j != args.end(); i++, j++) {
			auto &iv(*i), &jv(*j);
			auto it(iv.type()), jt(jv.type());
			if (it == env.no_type) { continue; }

			if (jv.is_undef()) {
				if (!it->isa(jt)) { return stdx::nullopt; }
			} else {
				if (iv.is_undef() || !iv.is_eqval(jv)) { return stdx::nullopt; }
			}
			
			score += std::abs(it->tag-jt->tag);
		}

		return score;
	}

	void Fimp::dump(std::ostream &out) const { out << id.name(); }
}
