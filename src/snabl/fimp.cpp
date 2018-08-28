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
		if (!fimp->imp) { throw Error(fmt("Missing imp: %0", {fimp->id})); }
		auto stack_offs(env.stack().size());
		auto &call(env.push_call(fimp, -1));
		(*fimp->imp)(call);
		auto func(fimp->func);
		
		if (env.stack().size() != stack_offs-func->nargs+func->nrets) {
			throw Error("Invalid stack after funcall");
		}
		
		env.pop_call();
	}

	Fimp::Fimp(const FuncPtr &func, const Args &args, const Rets &rets, Imp imp):
		id(get_id(func, args)), func(func), args(args), rets(rets), imp(imp),
		_is_compiled(false), _pc(0), _nops(0) { }

	Fimp::Fimp(const FuncPtr &func,
						 const Args &args, const Rets &rets,
						 Forms::const_iterator begin,
						 Forms::const_iterator end):
		id(get_id(func, args)), func(func), args(args), rets(rets), forms(begin, end),
		_is_compiled(false), _pc(0), _nops(0) { }

	bool Fimp::compile(Bin &bin, Pos pos) {
		if (_is_compiled) { return false; }
		auto &skip(bin.emplace_back(ops::Skip::type, pos, 0).as<ops::Skip>());
		_pc = bin.ops().size();
		const auto pc_backup(bin.pc());
		bin.emplace_back(ops::Begin::type, pos);
		bin.compile(forms);
		bin.emplace_back(ops::End::type, pos);
		bin.set_pc(pc_backup);
		_nops = skip.nops = bin.ops().size()-_pc;
		_is_compiled = true;
		return true;
	}

	std::size_t Fimp::pc() const { return _pc; }
	
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
