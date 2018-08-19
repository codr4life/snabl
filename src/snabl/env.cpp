#include <ctype.h>

#include <sstream>

#include "snabl/env.hpp"

namespace snabl {
	const Pos Env::home_pos(1, 0);
	
	Env::Env():
		_type_tag(1),
		lobby(*this, get_sym("lobby")),
		float_type(lobby.add_type<FloatType>(get_sym("Float"))),
		int_type(lobby.add_type<IntType>(get_sym("Int"))),
		separators({' ', '\t', '\n', '+', '-', '*', '/'}),
		bin(*this),
		main(begin(nullptr)),
		_pos(home_pos) { push_lib(&lobby); }

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
		parse(s, home_pos, out);		
	}
	
	void Env::parse(std::istream &in, Pos start_pos, Forms &out) {
		_pos = start_pos;
		char c;
		
		while (in.get(c)) {
			switch(c) {
			case ' ':
			case '\t':
				_pos.col++;
				break;	
			case '\n':
				_pos.row++;
				_pos.col = home_pos.col;
				break;
			default:
				if (isdigit(c)) {
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
	}
	
	void Env::parse_id(std::istream &in, Forms &out) {		
		std::stringstream buf;
		char c, pc(0);
		
		while (in.get(c)) {
			if (separators.find(c) == separators.end() ||
					(isgraph(c) && (!pc || pc == c))) {
				buf << c;
				_pos.col++;
			} else {
				in.putback(c);
				break;
			}

			pc = c;
		}

		out.emplace_back(forms::id_type, new forms::Id(get_sym(buf.str())));
	}

	void Env::parse_num(std::istream &in, Forms &out) {
		std::stringstream buf;
		bool is_float(false);
		char c;
		
		while (in.get(c)) {
			if (isdigit(c) || c == '.') {
				buf << c;
				_pos.col++;
				is_float |= c == '.';
			} else {
				in.putback(c);
				break;
			}
		}
	 
		auto f(new forms::Literal(is_float
															? Box(float_type, std::stold(buf.str()))
															: Box(int_type, std::stoll(buf.str()))));

		out.emplace_back(forms::literal_type, f);
	}
	
	void Env::compile(const std::string &in) {
		Forms forms;
		parse(in, forms);
		bin.compile(forms);
	}

	void Env::run(const std::string &in) {
		auto start_pc = bin.ops().size();
		compile(in);
		bin.run(start_pc);
	}

	void Env::push_lib(Lib *lib) {
		_libs.push_back(lib);
	}
	
	Lib *Env::lib() { return _libs.back(); }

	Lib *Env::pop_lib() {
		if (_libs.empty()) { throw Error("No libs"); }
		Lib *l(_libs.back());
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
