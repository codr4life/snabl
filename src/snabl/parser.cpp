#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/parser.hpp"

namespace snabl {
	const Pos Parser::init_pos(1, 0);
	
	void Parser::parse(istream &in, Forms &out) {
		parse(in, init_pos, 0, out);
	}

	bool Parser::parse(istream &in, Pos start_pos, char end, Forms &out) {
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
				_pos.col = init_pos.col;
				break;
			case ',':
				_pos.col++;
				return parse_body<forms::Comma>(in, end, out);
			case ';':
				_pos.col++;
				return parse_body<forms::Semi>(in, end, out);
			case '?': {
				_pos.col++;
				if (out.empty()) { throw Error("Nothing to query"); }
				auto form(out.back());
				out.pop_back();
				out.emplace_back(forms::Query::type, start_pos, form);
				break;
			}
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
	
	void Parser::parse_id(istream &in, Forms &out) {
		auto start_pos(_pos);
		stringstream buf;
		char c(0);
		bool prev_sep(true);
		
		while (in.get(c)) {
			bool c_sep(env.separators.find(c) != env.separators.end());
			
			if (!c_sep || (isgraph(c) && prev_sep)) {
				buf << c;
				_pos.col++;
			} else {
				break;
			}

			prev_sep = c_sep;
			c = 0;
		}

		const auto id(buf.str());
		
		if (id.front() == '$') {
			out.emplace_back(forms::Lit::type,
											 start_pos,
											 Box(env.sym_type, env.sym(id.substr(1))));
		} else {
			out.emplace_back(forms::Id::type, start_pos, env.sym(id));
			
			if (c == '<') {
				parse_type_list(in, out);
				c = 0;
			}
		}

		if (c) {
			in.clear();
			in.putback(c);
		}
	}

	void Parser::parse_lambda(istream &in, Forms &out) {
		auto start_pos(_pos);

		if (!parse_body<forms::Lambda>(in, '}', out)) {
			throw SyntaxError(start_pos, "Open lambda");
		}
	}

	void Parser::parse_num(istream &in, Forms &out) {
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
	 
		out.emplace_back(forms::Lit::type,
										 start_pos,
										 is_float
										 ? Box(env.float_type, stold(buf.str()))
										 : Box(env.int_type, stoll(buf.str())));
	}

	void Parser::parse_sexpr(istream &in, Forms &out) {
		auto start_pos(_pos);

		if (!parse_body<forms::Sexpr>(in, ')', out)) {
			throw SyntaxError(start_pos, "Open sexpr");
		}
	}

	void Parser::parse_type_list(istream &in, Forms &out) {
		auto start_pos(_pos);
		Forms body;

		if (!parse(in, start_pos, '>', body)) {
			throw SyntaxError(start_pos, "Open type list");
		}

		out.emplace_back(forms::TypeList::type, start_pos, body.cbegin(), body.cend());
	}

}
