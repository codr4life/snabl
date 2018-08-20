#include <ctype.h>

#include <sstream>

#include "snabl/env.hpp"

namespace snabl {
	const Pos Env::home_pos(1, 0);
	
	Env::Env():
		_type_tag(1),
		home(*this, get_sym("home")),
		a_type(home.add_type<Trait>(get_sym("A"))),
		num_type(home.add_type<Trait>(get_sym("Num"), {a_type})),
		float_type(home.add_type<FloatType>(get_sym("Float"), {num_type})),
		int_type(home.add_type<IntType>(get_sym("Int"), {num_type})),
		separators({' ', '\t', '\n', ',', '(', ')'}),
		bin(*this),
		main(begin(nullptr)),
		_pos(home_pos) { push_lib(home); }

	size_t Env::next_type_tag() { return _type_tag++; }

	Sym Env::get_sym(const std::string &name) {
		auto found(_syms.find(name));

		return Sym((found == _syms.end())
							 ? _syms.insert(std::make_pair(name, std::make_unique<SymImp>(name)))
							 .first->second.get()
							 : found->second.get());
	}

	void Env::parse(const std::string &in, Forms &out) {
		std::istringstream s(in);
		parse(s, home_pos, 0, out);		
	}
	
	bool Env::parse(std::istream &in, Pos start_pos, char end, Forms &out) {
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
				parse_rest(in, 0, out);
				break;
			case '(':
				_pos.col++;
				parse_rest(in, ')', out);
				break;
			default:
				bool is_num = isdigit(c);

				if (c == '-') {
					char nc;
					
					if (in.get(nc)) {
						in.putback(nc);
						is_num |= isdigit(nc);
					}
				}
				
				if (is_num) {
					in.putback(c);
					parse_num(in, out);
				} else if (isgraph(c)) {
					in.putback(c);
					parse_id(in, out);
				} else {
					throw Error("Invalid input");
				}
			}
		}

		return false;
	}
	
	void Env::parse_id(std::istream &in, Forms &out) {
		auto start_pos(_pos);
		std::stringstream buf;
		char c;
		bool prev_sep(true);
		
		while (in.get(c)) {
			bool c_sep(separators.find(c) != separators.end());
			
			if (!c_sep || (isgraph(c) && prev_sep)) {
				buf << c;
				_pos.col++;
			} else {
				in.putback(c);
				break;
			}

			prev_sep = c_sep;
		}

		out.emplace_back(forms::Id::type, start_pos, get_sym(buf.str()));
	}

	void Env::parse_num(std::istream &in, Forms &out) {
		auto start_pos(_pos);
		std::stringstream buf;
		bool is_float(false);
		char c;
		
		while (in.get(c)) {
			if (isdigit(c) || c == '-' || c == '.') {
				buf << c;
				_pos.col++;
				is_float |= c == '.';
			} else {
				in.putback(c);
				break;
			}
		}
	 
		out.emplace_back(forms::Literal::type,
										 start_pos,
										 is_float
										 ? Box(float_type, std::stold(buf.str()))
										 : Box(int_type, std::stoll(buf.str())));
	}

	void Env::parse_rest(std::istream &in, char end, Forms &out) {
		auto start_pos(_pos);
		Forms body;
		parse(in, start_pos, end, body);
		out.emplace_back(forms::Sexpr::type, start_pos, std::move(body));
	}

	void Env::compile(const std::string &in) {
		Forms forms;
		parse(in, forms);
		bin.compile(forms);
	}

	void Env::run(const std::string &in) {
		size_t start_pc = bin.ops().size();
		compile(in);
		bin.run(start_pc);
	}

	void Env::push_lib(Lib &lib) {
		_libs.push_back(&lib);
	}
	
	Lib &Env::lib() {
		if (_libs.empty()) { throw Error("No libs"); }
		return *_libs.back();
	}

	Lib &Env::pop_lib() {
		if (_libs.empty()) { throw Error("No libs"); }
		Lib &l(*_libs.back());
		_libs.pop_back();
		return l;
	}

	ScopePtr Env::begin(const ScopePtr &parent) {
		auto s(std::make_shared<Scope>(*this, parent));
		_scopes.push_back(s);
		return s;
	}

	ScopePtr Env::scope() {
		if (_scopes.empty()) { throw Error("No open scopes"); }
		return _scopes.back();
	}

	ScopePtr Env::end() {
		auto s(_scopes.back());
		_scopes.pop_back();
		return s;
	}

	Call &Env::push_call(const TargetPtr &target, ssize_t return_pc) {
		_calls.emplace_back(target, scope(), return_pc);
		return _calls.back();
	}
	
	Call *Env::peek_call() { return _calls.empty() ? nullptr : &_calls.back(); }

	void Env::pop_call() { _calls.pop_back(); }
	
	void Env::push_stack(const Box &value) { _stack.push_back(value); }

	std::optional<Box> Env::pop_stack() {
		if (_stack.empty()) { return std::nullopt; }
		Box v(_stack.back());
		_stack.pop_back();
		return v;
	}

	const Stack &Env::stack() { return _stack; }
}
