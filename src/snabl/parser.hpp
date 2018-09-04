#ifndef SNABL_PARSER_HPP
#define SNABL_PARSER_HPP

#include "snabl/form.hpp"
#include "snabl/pos.hpp"

namespace snabl {
	class Env;

	class Parser {
	public:
		static const Pos init_pos;
		Env &env;
		
		Parser(Env &env): env(env), _pos(init_pos) { }
		void parse(istream &in, Forms &out);
		bool parse(istream &in, Pos start_pos, char end, Forms &out);		
	private:
		Pos _pos;

		template <typename FormT>
		bool parse_body(istream &in, char end, Forms &out) {
			auto start_pos(_pos);
			Forms body;
			if (!parse(in, start_pos, end, body) && end) { return false; }
			out.emplace_back(FormT::type, start_pos, body.cbegin(), body.cend());
			return true;
		}

		void parse_id(istream &in, Forms &out);
		void parse_lambda(istream &in, Forms &out);
		void parse_num(istream &in, Forms &out);
		void parse_sexpr(istream &in, Forms &out);
		void parse_type_list(istream &in, Forms &out);		
	};
}

#endif
