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

	void Env::compile(const Forms &forms) { compile(forms.begin(), forms.end()); }
	
	void Env::compile(const Form &form) {
		Forms fs {form};
		auto i(fs.cbegin());
		i->imp->compile(i, i+1, *this);
	}

	void Env::compile(Forms::const_iterator begin, Forms::const_iterator end) {
		for (auto i(begin); i != end;) { i->imp->compile(i, end, *this); }
	}
}
