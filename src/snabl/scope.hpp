#ifndef SNABL_SCOPE_HPP
#define SNABL_SCOPE_HPP

#include "snabl/ptrs.hpp"
#include "snabl/stack.hpp"
#include "snabl/std.hpp"
#include "snabl/sym.hpp"

namespace snabl {
	class Env;

	class Scope {
	public:
		static const Int MaxRegs = 8;
		
		ScopePtr prev;
		const ScopePtr source;
		
		Scope(const ScopePtr &prev, const ScopePtr &source):
			prev(prev), source(source) { }

		Scope(const Scope &) = delete;
		const Scope &operator=(const Scope &) = delete;
		
		const Box *get(Sym id) const {
			const auto found(_vars.find(id));
			if (found != _vars.end()) { return &found->second; }
			return source ? source->get(id) : nullptr;
		}

		void let(Sym id, const Box &val) {
			const auto ok(_vars.emplace(make_pair(id, val)));
			if (!ok.second) { throw Error("Duplicate var: " + id.name()); }
		}

		void clear_vars() { _vars.clear(); }

		template <typename T>
		const T &get_reg(Int idx) const { return any_cast<const T &>(_regs[idx]); }

		void let_reg(Int idx, any &&val) { _regs[idx] = val; }
		void clear_reg(Int idx) { _regs[idx].reset(); }
	private:
		array<any, MaxRegs> _regs;
		map<Sym, Box> _vars;
	};
}

#endif
