#include <ctype.h>

#include "snabl/call.hpp"
#include "snabl/env.hpp"

namespace snabl {
	const Pos Env::home_pos(1, 0);

	void Env::parse(string_view in, Forms &out) {
		const string s(in);
		istringstream ss(s);
		parse(ss, out);		
	}	

	void Env::parse(istream &in, Forms &out) {
		parse(in, home_pos, 0, out);
	}
	
	bool Env::parse(istream &in, Pos start_pos, char end, Forms &out) {
		_pos = start_pos;
		char c;
		
		while (in.get(c)) {
			if (c == end) {
				_pos.col++;
				return true;
			}
			
			switch(c) {
			case ' ':
			case '\t':
				_pos.col++;
				break;	
			case '\n':
				_pos.row++;
				_pos.col = home_pos.col;
				break;
			case ',':
				_pos.col++;
				return parse_body<forms::Comma>(in, end, out);
			case ';':
				_pos.col++;
				return parse_body<forms::Semicolon>(in, end, out);
			case '(':
				_pos.col++;
				parse_sexpr(in, out);
				break;
			case '{':
				_pos.col++;
				parse_lambda(in, out);
				break;
			default:
				bool is_num = isdigit(c);

				if (c == '-') {
					char nc;
					
					if (in.get(nc)) {
						in.clear();
						in.putback(nc);
						is_num |= isdigit(nc);
					}
				}
				
				if (is_num) {
					in.clear();
					in.putback(c);
					parse_num(in, out);
				} else if (isgraph(c)) {
					in.clear();
					in.putback(c);
					parse_id(in, out);
				} else {
					throw Error("Invalid input");
				}
			}
		}

		return false;
	}
	
	void Env::parse_id(istream &in, Forms &out) {
		auto start_pos(_pos);
		stringstream buf;
		char c(0);
		bool prev_sep(true);
		
		while (in.get(c)) {
			bool c_sep(separators.find(c) != separators.end());
			
			if (!c_sep || (isgraph(c) && prev_sep)) {
				buf << c;
				_pos.col++;
			} else {
				break;
			}

			prev_sep = c_sep;
			c = 0;
		}

		out.emplace_back(forms::Id::type, start_pos, sym(buf.str()));

		if (c == '<') {
			parse_type_list(in, out);
		} else if (c) {
			in.clear();
			in.putback(c);
		}
	}

	void Env::parse_lambda(istream &in, Forms &out) {
		auto start_pos(_pos);

		if (!parse_body<forms::Lambda>(in, '}', out)) {
			throw SyntaxError(start_pos, "Open lambda");
		}
	}

	void Env::parse_num(istream &in, Forms &out) {
		auto start_pos(_pos);
		stringstream buf;
		bool is_float(false);
		char c;
		
		while (in.get(c)) {
			if (isdigit(c) || c == '-' || c == '.') {
				buf << c;
				_pos.col++;
				is_float |= c == '.';
			} else {
				in.clear();
				in.putback(c);
				break;
			}
		}
	 
		out.emplace_back(forms::Literal::type,
										 start_pos,
										 is_float
										 ? Box(float_type, stold(buf.str()))
										 : Box(int_type, stoll(buf.str())));
	}

	void Env::parse_sexpr(istream &in, Forms &out) {
		auto start_pos(_pos);

		if (!parse_body<forms::Sexpr>(in, ')', out)) {
			throw SyntaxError(start_pos, "Open sexpr");
		}
	}

	void Env::parse_type_list(istream &in, Forms &out) {
		auto start_pos(_pos);
		Forms body;

		if (!parse(in, start_pos, '>', body)) {
			throw SyntaxError(start_pos, "Open type list");
		}

		out.emplace_back(forms::TypeList::type, start_pos, body.cbegin(), body.cend());
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
