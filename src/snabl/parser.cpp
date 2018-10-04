#include "snabl/env.hpp"
#include "snabl/error.hpp"
#include "snabl/parser.hpp"
#include "snabl/types.hpp"

namespace snabl {
  const Pos Parser::init_pos(1, 0);
  
  void Parser::parse(istream &in, Forms &out) {
    parse(in, init_pos, 0, out);
  }

  bool Parser::parse(istream &in, Pos start_pos, char end, Forms &out) {
    while (!in.eof()) {
      if (parse_form(in, end, out)) { return true; }
    }

    return false;
  }
  
  bool Parser::parse_form(istream &in, char end, Forms &out) {
    char c;
    if (!in.get(c)) { return false; }
    
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
      return parse_body<forms::More>(in, end, out);
    case '|':
      _pos.col++;
      return parse_body<forms::Split>(in, end, out);
    case '?': {
      if (out.empty()) { throw CompileError(_pos, "Nothing to query"); }
      auto form(out.back());
      out.pop_back();
      out.emplace_back(forms::Query::type, _pos, form);
      _pos.col++;
      break;
    }
    case '&': {
      auto start(_pos);
      _pos.col++;       
      parse_form(in, end, out);
      if (out.empty()) { throw CompileError(start, "Nothing to ref"); }
      auto form(out.back());
      out.pop_back();
      out.emplace_back(forms::Ref::type, start, form);
      break;
    }
    case '(':
      _pos.col++;
      parse_sexpr(in, out);
      break;
    case '{':
      _pos.col++;
      parse_scope(in, out);
      break;
    case '[':
      _pos.col++;
      parse_stack(in, out);
      break;
    case '~':
      _pos.col++;
      parse_special_char(in, out);
      break;
    case '#':
      _pos.col++;
      parse_char(in, out);
      break;
    case '\'': {
      char nc(0);
        
      if (in.get(nc) && nc == '\'') {
        _pos.col += 2;
        parse_str(in,out);
      } else {
        in.clear();
        if (nc) { in.putback(nc); }
        in.putback(c);
        parse_id(in, out);
      }

      break;
    }
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
  
    return false;
  }
  
  void Parser::parse_id(istream &in, Forms &out) {
    const auto start_pos(_pos);
    stringstream buf;
    char c, pc(0);
    bool pc_sep;
    
    while (in.get(c)) {
      bool c_sep(env.separators.find(c) != env.separators.end());
      
      if (c != ' ' && c != '\n' && (!pc || (!c_sep && !pc_sep) || c == pc)) {
        buf << c;
        _pos.col++;
      } else {
        break;
      }

      pc = c;
      pc_sep = c_sep;
      c = 0;
    }

    const auto id(buf.str());
    
    if (id.front() == '\'') {
      out.emplace_back(forms::Lit::type,
                       start_pos,
                       Box(env.sym_type, env.sym(id.substr(1))));
    } else if (c == '<') {
      parse_fimp(start_pos, env.sym(buf.str()), in, out);
      c = 0;
    } else {
      out.emplace_back(forms::Id::type, start_pos, env.sym(id));
    }

    if (c) {
      in.clear();
      in.putback(c);
    }
  }

  void Parser::parse_scope(istream &in, Forms &out) {
    const auto start_pos(_pos);

    if (!parse_body<forms::Scope>(in, '}', out)) {
      throw SyntaxError(start_pos, "Open scope");
    }
  }

  void Parser::parse_num(istream &in, Forms &out) {
    const auto start_pos(_pos);
    stringstream buf;
    bool is_float(false);
    char c;
    
    while (in.get(c)) {
      if (c == '.') {
        char nc;
        
        if (in.get(nc) && nc == '.') {
          in.clear();
          in.putback('.');
          in.putback('.');
          break;
        }

        is_float = true;
        buf << '.';
        _pos.col++;
      } else if (isdigit(c) || c == '-') {
        buf << c;
        _pos.col++;
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
    const auto start_pos(_pos);

    if (!parse_body<forms::Sexpr>(in, ')', out)) {
      throw SyntaxError(start_pos, "Open sexpr");
    }
  }

  void Parser::parse_stack(istream &in, Forms &out) {
    const auto start_pos(_pos);

    if (!parse_body<forms::Stack>(in, ']', out)) {
      throw SyntaxError(start_pos, "Open stack");
    }
  }
  
  void Parser::parse_special_char(istream &in, Forms &out) {
    auto p(_pos);
    p.col--;

    char c(0);
    if (!in.get(c)) { throw SyntaxError(p, "Missing special char"); }
    auto sc(env.find_special_char(c));
    if (!sc) { throw SyntaxError(p, fmt("Unknown special char: %0", {c})); }
    out.emplace_back(forms::Lit::type, p, Box(env.char_type, *sc));
  }

  void Parser::parse_char(istream &in, Forms &out) {
    auto p(_pos);
    p.col--;

    char c(0);
    if (!in.get(c)) { throw SyntaxError(p, "Missing char"); }
    out.emplace_back(forms::Lit::type, p, Box(env.char_type, Char(c)));
  }

  void Parser::parse_str(istream &in, Forms &out) {
    auto p(_pos);
    p.col--;
    stringstream s;
    char c(0);
    
    while (in.get(c)) {
      if (c == '\'') {
        char nc(0);
        
        if (in.get(nc) && nc == '\'') {
          _pos.col++;
          break;
        }
        else {
          s << c;
          
          if (nc) {
            _pos.col++;
            s << nc;
          }
        }

        _pos.col++;
      } else {
        s << c;
      }

      _pos.col++;
      c = 0;
    }

    if (!c) { throw SyntaxError(p, "Open string"); }
    
    out.emplace_back(forms::Lit::type, p,
                     Box(env.str_type, make_shared<Str>(s.str())));
  }

  void Parser::parse_fimp(Pos pos, Sym id, istream &in, Forms &out) {
    Forms body;
    if (!parse(in, pos, '>', body)) { throw SyntaxError(pos, "Open fimp"); }
    out.emplace_back(forms::Fimp::type, pos, id, body.cbegin(), body.cend());
  }

}
