#ifndef SNABL_PARSER_HPP
#define SNABL_PARSER_HPP

#include "snabl/form.hpp"
#include "snabl/pos.hpp"

namespace snabl {
  struct Env;

  struct Parser {
    static const Pos home;
    Env &env;
    Pos pos;
    
    Parser(Env &env): env(env), pos(home) { }
    void parse(istream &in, Forms &out);
    bool parse(istream &in, Pos start_pos, char end, Forms &out);   
    bool parse_form(istream &in, char end, Forms &out);

    template <typename FormT>
    bool parse_body(istream &in, char end, Forms &out) {
      auto start_pos(pos);
      Forms body;
      if (!parse(in, start_pos, end, body) && end) { return false; }
      out.emplace_back(FormT::type, start_pos, body.cbegin(), body.cend());
      return true;
    }

    void parse_id(istream &in, Forms &out);
    void parse_num(istream &in, Forms &out);
    void parse_scope(istream &in, Forms &out);
    void parse_sexpr(istream &in, Forms &out);
    void parse_stack(istream &in, Forms &out);
    void parse_special_char(istream &in, Forms &out);
    void parse_char(istream &in, Forms &out);
    void parse_str(istream &in, Forms &out);
    void parse_fimp(Pos pos, Sym id, istream &in, Forms &out);
  };
}

#endif
