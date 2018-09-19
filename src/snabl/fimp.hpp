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
		using Imp = function<void (const FimpPtr &)>;
		
		const FuncPtr func;
		const Args args;
		const optional<Form> form;
		const Imp imp;

		static Sym get_id(const Func &func, const Args &args);
		static bool compile(const FimpPtr &fimp, Pos pos);
		static void call(const FimpPtr &fimp, Pos pos);

		Fimp(const FuncPtr &func, const Args &args, Imp imp);
		Fimp(const FuncPtr &func, const Args &args, const Form &form);

		optional<size_t> score(Stack::const_iterator begin,
													 Stack::const_iterator end) const;
	private:
		bool _is_calling;

		friend Env;
		friend ops::Fimp;
		friend ops::FimpEnd;
	};
}

#endif
