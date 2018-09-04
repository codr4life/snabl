#include <ctype.h>

#include "snabl/env.hpp"
#include "snabl/parser.hpp"

namespace snabl {
	void Env::parse(string_view in, Forms &out) {
		const string s(in);
		istringstream ss(s);
		Parser(*this).parse(ss, out);		
	}	

	void Env::compile(string_view in) {
		Forms forms;
		parse(in, forms);
		compile(forms);
	}

	void Env::emit(Pos pos, FuncPtr &func, FimpPtr &fimp) {		
		if (fimp) {
			if (!fimp->imp) { fimp->compile(pos); }
			emit(ops::Funcall::type, pos, fimp);
			fimp = nullptr;
		} else if (func) {
			emit(ops::Funcall::type, pos, func);
		}

		func = nullptr;
	}
	
	void Env::compile(const Forms &forms) { compile(forms.begin(), forms.end()); }

	void Env::compile(Forms::const_iterator begin, Forms::const_iterator end) {
		FuncPtr func;
		FimpPtr fimp;

		compile(begin, end, func, fimp);
	}
	
	void Env::compile(Forms::const_iterator begin, Forms::const_iterator end,
										FuncPtr &func, FimpPtr &fimp) {
		for (auto i(begin); i != end;) { i->imp->compile(i, end, func, fimp, *this); }
		emit(begin->pos, func, fimp);
	}
}
