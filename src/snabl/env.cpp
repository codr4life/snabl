#include <ctype.h>

#include <sstream>

#include "snabl/env.hpp"

namespace snabl {
	const Pos Env::home_pos(1, 0);
	
	Env::Env():
		_type_tag(1),
		home(*this),
		separators({' ', '\t', '\n', ',', '<', '>', '(', ')'}),
		bin(*this),
		main(begin()),
		_pos(home_pos) { push_lib(home); }

	std::size_t Env::next_type_tag() { return _type_tag++; }

	Sym Env::get_sym(const std::string &name) {
		auto found(_syms.find(name));

		return Sym((found == _syms.end())
							 ? _syms.emplace(name, std::make_unique<SymImp>(name))
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
				parse_sexpr(in, out);
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
				break;
			}

			prev_sep = c_sep;
		}

		out.emplace_back(forms::Id::type, start_pos, get_sym(buf.str()));

		if (c == '<') {
			parse_type_list(in, out);
		} else if (c) {
			in.putback(c);
		}
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

	bool Env::parse_rest(std::istream &in, char end, Forms &out) {
		auto start_pos(_pos);
		Forms body;
		if (!parse(in, start_pos, end, body) && end) { return false; }
		out.emplace_back(forms::Sexpr::type, start_pos, std::move(body));
		return true;
	}

	void Env::parse_sexpr(std::istream &in, Forms &out) {
		auto start_pos(_pos);
		if (!parse_rest(in, ')', out)) { throw SyntaxError(start_pos, "Open sexpr"); }
	}

	void Env::parse_type_list(std::istream &in, Forms &out) {
		auto start_pos(_pos);
		Forms body;

		if (!parse(in, start_pos, '>', body)) {
			throw SyntaxError(start_pos, "Open type list");
		}

		out.emplace_back(forms::TypeList::type, start_pos, body);
	}
	
	void Env::compile(const std::string &in) {
		Forms forms;
		parse(in, forms);
		bin.compile(forms);
	}

	void Env::run(const std::string &in) {
		std::size_t start_pc = bin.ops().size();
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

	ScopePtr Env::begin() {
		auto s(std::make_shared<Scope>(*this));
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

	Call &Env::push_call(const TargetPtr &target,
											 std::optional<std::size_t> return_pc) {
		_calls.emplace_back(target, scope(), return_pc);
		return _calls.back();
	}
	
	Call *Env::peek_call() { return _calls.empty() ? nullptr : &_calls.back(); }

	void Env::pop_call() { _calls.pop_back(); }
	
	void Env::push_stack(const Box &value) { _stack.push_back(value); }

	std::optional<Box> Env::unsafe_pop_stack() {
		if (_stack.empty()) { return std::nullopt; }
		Box v(_stack.back());
		_stack.pop_back();
		return v;
	}

	const Stack &Env::stack() { return _stack; }

	Box const* Env::get_var(Sym id) {
		auto found(_vars.find(id));
		if (found != _vars.end()) { return &found->second; }
		return nullptr;
	}

	std::optional<Box> Env::unsafe_put_var(Sym id, const Box &value) {
		auto found(_vars.find(id));
		std::optional<Box> prev;
		
		if (found != _vars.end()) {
			prev = found->second;
			found->second = value;
		} else {
			_vars.emplace(std::make_pair(id, value)).first->second;
		}

		return prev;
	}
}
