#include <ctype.h>

#include "snabl/env.hpp"
#include "snabl/parser.hpp"

namespace snabl {
	void Env::compile(string_view in) {
		const string s(in);
		istringstream ss(s);
		compile(ss);
	}

	void Env::compile(istream &in) {
		Forms forms;
		Parser(*this).parse(in, forms);		
		compile(forms);
	}

	void Env::emit(Pos pos, FuncPtr &func, FimpPtr &fimp) {		
		if (fimp) {
			if (!fimp->imp) { Fimp::compile(fimp, pos); }
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
