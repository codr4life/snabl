#include <ctype.h>

#include "snabl/env.hpp"

#define SNABL_DISPATCH()												\
	if (pc == *end_pc) { return; }								\
	goto *op_labels[pc->type.label_offs];					\

namespace snabl {
	const Pos Env::home_pos(1, 0);
	
	Env::Env():
		_type_tag(1),
		home(*this),
		separators({' ', '\t', '\n', ',', '<', '>', '(', ')'}),
		main(begin_scope()),
		_pos(home_pos) { begin_lib(home); }

	size_t Env::next_type_tag() { return _type_tag++; }

	Sym Env::sym(const string &name) {
		auto found(_syms.find(name));

		return Sym((found == _syms.end())
							 ? _syms.emplace(name, make_unique<SymImp>(name))
							 .first->second.get()
							 : found->second.get());
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
				return parse_rest(in, end, out);
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

	void Env::parse(string_view in, Forms &out) {
		const string s(in);
		istringstream ss(s);
		parse(ss, home_pos, 0, out);		
	}	
	
	void Env::parse_id(istream &in, Forms &out) {
		auto start_pos(_pos);
		stringstream buf;
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

		out.emplace_back(forms::Id::type, start_pos, sym(buf.str()));

		if (c == '<') {
			parse_type_list(in, out);
		} else if (c) {
			in.putback(c);
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

	bool Env::parse_rest(istream &in, char end, Forms &out) {
		auto start_pos(_pos);
		Forms body;
		if (!parse(in, start_pos, end, body) && end) { return false; }
		out.emplace_back(forms::Sexpr::type, start_pos, move(body));
		return true;
	}

	void Env::parse_sexpr(istream &in, Forms &out) {
		auto start_pos(_pos);
		if (!parse_rest(in, ')', out)) { throw SyntaxError(start_pos, "Open sexpr"); }
	}

	void Env::parse_type_list(istream &in, Forms &out) {
		auto start_pos(_pos);
		Forms body;

		if (!parse(in, start_pos, '>', body)) {
			throw SyntaxError(start_pos, "Open type list");
		}

		out.emplace_back(forms::TypeList::type, start_pos, body);
	}
	
	void Env::compile(string_view in) {
		Forms forms;
		parse(in, forms);
		compile(forms);
	}

	void Env::compile(const Forms &forms) { compile(forms.begin(), forms.end()); }

	void Env::compile(const Forms::const_iterator &begin,
										const Forms::const_iterator &end) {
		FuncPtr func;
		FimpPtr fimp;

		for (auto i(begin); i != end;) { i->imp->compile(i, end, func, fimp, *this); }
		const auto pos(begin->pos);

		if (fimp) {
			if (!fimp->imp) { Fimp::compile(fimp, pos); }
			emit(ops::Funcall::type, pos, fimp);
		} else if (func) {
			emit(ops::Funcall::type, pos, func);
		}
	}

	void Env::run(string_view in) {
		auto offs(ops.size());
		compile(in);
		pc = ops.begin()+offs;		
		run();
	}

	void Env::run(optional<PC> end_pc) {
		if (!end_pc) { end_pc = ops.end(); }
		
		static void* op_labels[] = {
			&&op_begin, &&op_drop, &&op_else, &&op_end, &&op_fimp, &&op_funcall,
			&&op_getvar, &&op_push, &&op_putvar, &&op_return, &&op_skip
		};

		SNABL_DISPATCH();
	op_begin:
		begin_scope();
		pc++;
		SNABL_DISPATCH();
	op_drop:
		pop();
		pc++;
		SNABL_DISPATCH();
	op_else: {
			const auto &op(pc->as<ops::Else>());
			const auto v(pop());
			if (!v.as<bool>()) { pc += op.nops; }
			pc++;
			SNABL_DISPATCH();
		}
	op_end:
		end_scope();
		pc++;
		SNABL_DISPATCH();
	op_fimp:
		pc++;
		SNABL_DISPATCH();		
	op_funcall: {
			auto &op(pc->as<ops::Funcall>());
			FimpPtr fimp(op.fimp);
			
			if (!fimp && op.prev_fimp) { fimp = op.prev_fimp; }
			
			if (fimp) {
				if (!fimp->score(_stack)) { fimp = nullptr; }
			} else {
				fimp = op.func->get_best_fimp(_stack);
			}
			
			if (!fimp) { throw Error(fmt("Func not applicable: %0", {op.func->id})); }

			if (!op.fimp) { op.prev_fimp = fimp; }
			if (Fimp::call(fimp, pc->pos)) { pc++; }
			SNABL_DISPATCH();
		}
	op_getvar: {
			const auto &op(pc->as<ops::GetVar>());		
			auto v(scope()->get_var(op.id));
			if (!v) { throw Error("Unknown var"); }
			push(*v);
			pc++;
			SNABL_DISPATCH();
		}
	op_push:
		push(pc->as<ops::Push>().value); 
		pc++;
		SNABL_DISPATCH();
	op_putvar: {
			const auto &op(pc->as<ops::PutVar>());
			auto v(pop());
			scope()->put_var(op.id, v);
			pc++;
			SNABL_DISPATCH();
		}
	op_return: {
			const auto call(end_call());
			const auto fn(dynamic_pointer_cast<Fimp>(call.target)->func);
			end_scope();
			if (_stack.size() < fn->nrets) { throw Error("Nothing to return"); }
			if (!call.return_pc) { throw Error("Missing return pc"); }
			pc = *call.return_pc;
			SNABL_DISPATCH();
		}
	op_skip:
		pc += pc->as<ops::Skip>().nops+1;
		SNABL_DISPATCH();
	}

	void Env::begin_lib(Lib &lib) {
		_libs.push_back(&lib);
	}
	
	Lib &Env::lib() {
		if (_libs.empty()) { throw Error("No libs"); }
		return *_libs.back();
	}

	Lib &Env::end_lib() {
		if (_libs.empty()) { throw Error("No libs"); }
		Lib &l(*_libs.back());
		_libs.pop_back();
		return l;
	}

	ScopePtr Env::begin_scope() {
		auto s(make_shared<Scope>(*this));
		_scopes.push_back(s);
		return s;
	}

	ScopePtr Env::scope() {
		if (_scopes.empty()) { throw Error("No open scopes"); }
		return _scopes.back();
	}

	ScopePtr Env::end_scope() {
		auto s(_scopes.back());
		_scopes.pop_back();
		return s;
	}

	Call &Env::begin_call(const CallTargetPtr &target,
												optional<Ops::iterator> return_pc) {
		_calls.emplace_back(target, scope(), return_pc);
		return _calls.back();
	}
	
	Call *Env::call() { return _calls.empty() ? nullptr : &_calls.back(); }

	Call Env::end_call() {
		if (_calls.empty()) { throw Error("No active calls"); }
		auto c(_calls.back());
		_calls.pop_back();
		return c;
	}
	
	void Env::push(const Box &value) { _stack.push_back(value); }

	Box Env::pop() {
		if (_stack.empty()) { throw Error("Nothing to pop"); }
		Box v(_stack.back());
		_stack.pop_back();
		return v;
	}

	const Stack &Env::stack() { return _stack; }

	void Env::dump_stack(std::ostream &out) const {
		out << '[';
		char sep(0);
		
		for (auto &v: _stack) {
			if (sep) { out << sep; }
			v.dump(out);
			sep = ' ';
		}

		out << ']' << endl;
	}
}
