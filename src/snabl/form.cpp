#include "snabl/env.hpp"
#include "snabl/form.hpp"

namespace snabl {
  AFormType::AFormType(string_view id): id(id) { }

  FormImp::~FormImp() { }

  Form::Form(const Form &source):
    type(source.type),
    pos(source.pos),
    imp(source.imp->clone()) { }

  namespace forms {
    const FormType<Fimp> Fimp::type("fimp");
    const FormType<Id> Id::type("id");
    const FormType<Lit> Lit::type("lit");
    const FormType<Query> Query::type("query");
    const FormType<Ref> Ref::type("ref");
    const FormType<Rest> Rest::type("rest");
    const FormType<Scope> Scope::type("scope");
    const FormType<Sexpr> Sexpr::type("sexpr");
    const FormType<Split> Split::type("split");
    const FormType<Stack> Stack::type("stack");

    void Body::dump(ostream &out) const {
      char sep(0);
      
      for (auto &f: body) {
        if (sep) { out << sep; }
        f.imp->dump(out);
        sep = ' ';
      }
    }   

    Fimp::Fimp(Sym id,
               Forms::const_iterator begin,
               Forms::const_iterator end): id(id) {
      transform(begin, end, back_inserter(type_ids),
                [](const Form &f) -> Sym { return f.as<Id>().id; });
    }

    Fimp::Fimp(Sym id, const Ids &type_ids): id(id), type_ids(type_ids) { }

    FormImp *Fimp::clone() const { return new Fimp(id, type_ids); }

    void Fimp::dump(ostream &out) const {
      out << id << '<';
      char sep(0);
      
      for (auto &id: type_ids) {
        if (sep) { out << sep; }
        out << id.name();
        sep = ' ';
      }
      
      out << '>';
    }
    
    void Fimp::compile(Forms::const_iterator &in,
                       Forms::const_iterator end,
                       Env &env) const {
      auto &lib(env.lib());
      auto pos(in->pos);
      auto &form((in++)->as<Fimp>());
      snabl::Stack args;
        
      transform(form.type_ids.begin(), form.type_ids.end(), back_inserter(args),
                [&env, &lib, pos](Sym id) {
                  auto &idn(id.name());

                  auto t((idn.size() == 1 && idn[0] == '_')
                         ? &env.no_type
                         : lib.get_type(id));
                  
                  if (!t) {
                    throw CompileError(pos, "Unknown type: " + id.name());
                  }
                  
                  return Box(*t);
                });
      
      auto fn(lib.get_func(id));
      if (!fn) { throw CompileError(pos, fmt("Unknown func: '%0'", {id.name()})); }
      
      if (I64(args.size()) != fn->nargs) {
        throw CompileError(pos, fmt("Wrong number of args: %0", {fn->id}));
      }
      
      auto fip(fn->get_best_fimp(args.begin(), args.end()));
      if (!fip) { throw CompileError(pos, fmt("Unknown fimp: %0", {fn->id})); }

      auto &fi(*fip);               
      if (!fi.imp) { fi.compile(pos); }
      env.emit<ops::Funcall>(pos, fi);
    }

    Id::Id(Sym id): id(id) { }

    FormImp *Id::clone() const { return new Id(id); }

    void Id::dump(ostream &out) const { out << id.name(); }
    
    void Id::compile(Forms::const_iterator &in,
                     Forms::const_iterator end,
                     Env &env) const {
      auto &form(*in);
      auto id(form.as<Id>().id);
      auto &idn(id.name());
      auto found_lib(idn.rfind('.'));
      Lib *lib(&env.lib());
      
      if (found_lib != string::npos && found_lib > 0 && idn[found_lib-1] != '.') {
        const auto lib_id(env.sym(idn.substr(0, found_lib)));
        lib = env.get_lib(lib_id);
        if (!lib) { throw CompileError(form.pos, fmt("Unknown lib: %0", {lib_id})); }
        id = env.sym(idn.substr(found_lib+1));
      }
      
      if (idn.front() == '@') {
        in++;
        env.emit<ops::Get>(form.pos, env.sym(idn.substr(1)));
      } else if (isupper(idn.front())) {
        in++;
        auto t(lib->get_type(id));
        if (!t) { throw CompileError(form.pos, fmt("Unknown type: %0", {id})); }
        env.emit<ops::Push>(form.pos, env.meta_type, t);
      } else {
        auto m(lib->get_macro(id));
        
        if (m) {
          m->call(in, end, env);
        } else {
          in++;
          auto fn(lib->get_func(id));

          if (!fn) {
            throw CompileError(form.pos, fmt("Unknown id: '%0'", {id}));
          }
          
          env.emit<ops::Funcall>(form.pos, *fn);
        }
      }
    }
    
    Lit::Lit(const Box &val): val(val) { }

    FormImp *Lit::clone() const { return new Lit(val); }

    void Lit::dump(ostream &out) const { val.dump(out); }

    void Lit::compile(Forms::const_iterator &in,
                      Forms::const_iterator end,
                      Env &env) const {
      auto &form(*in++);
      env.emit<ops::Push>(form.pos, form.as<Lit>().val);
    }

    Query::Query(const Form &form): form(form) {}
    
    FormImp *Query::clone() const {
      return new Query(form);
    }

    void Query::dump(ostream &out) const {
      form.imp->dump(out);
      out << '?';
    }   

    void Query::compile(Forms::const_iterator &in,
                        Forms::const_iterator end,
                        Env &env) const {
      auto &form(*in);
      auto &qf(form.as<forms::Query>().form);
      
      if (&qf.type == &forms::Lit::type) {
        env.emit<ops::Eqval>(qf.pos, qf.as<Lit>().val);
      } else if (&qf.type == &forms::Id::type) {
        if (isupper(qf.as<forms::Id>().id.name().front())) {
          auto &id(qf.as<forms::Id>().id);
          auto t(env.lib().get_type(id));
          if (!t) { throw CompileError(qf.pos, fmt("Unknown type: %0", {id})); }
          env.emit<ops::Is>(qf.pos, *t);
        } else {
          env.compile(qf);
          env.emit<ops::Eqval>(qf.pos);
        }
      } else {
        throw CompileError(qf.pos, fmt("Invalid query: %0", {qf.type.id}));
      }

      in++;
    }

    Ref::Ref(const Form &form): form(form) {}
    
    FormImp *Ref::clone() const {
      return new Ref(form);
    }

    void Ref::dump(ostream &out) const {
      out << '&';
      form.imp->dump(out);
    }   

    void Ref::compile(Forms::const_iterator &in,
                      Forms::const_iterator end,
                      Env &env) const {
      auto &f(*in++);
      auto &rf(f.as<forms::Ref>().form);
      
      if (&rf.type == &Scope::type || &rf.type == &Sexpr::type) {
        const auto is_scope(&rf.type == &Scope::type);
        auto &start(env.emit<ops::Lambda>(f.pos, is_scope));
        if (is_scope) { env.begin_regs(); }
        env.compile(rf.as<Body>().body);
        if (is_scope) { env.end_regs(); }
        env.emit<ops::Return>(f.pos);
        start.start_pc = start.next;
        start.end_pc = env.ops.size();
      } else {
        throw CompileError(rf.pos, fmt("Invalid ref: %0", {rf.type.id}));
      }
    }
    
    FormImp *Rest::clone() const { return new Rest(body.begin(), body.end()); }

    void Rest::dump(ostream &out) const {
      out << ", ";
      Body::dump(out);
    }   

    void Rest::compile(Forms::const_iterator &in,
                       Forms::const_iterator end,
                       Env &env) const {
      env.compile((in++)->as<Rest>().body);
    }

    FormImp *Scope::clone() const { return new Scope(body.begin(), body.end()); }

    void Scope::dump(ostream &out) const {
      out << '{';
      Body::dump(out);
      out << '}';
    }   

    void Scope::compile(Forms::const_iterator &in,
                        Forms::const_iterator end,
                        Env &env) const {
      auto &f(*in++);
      auto &sf(f.as<Scope>());
      env.emit<ops::Scope>(f.pos);
      env.begin_regs();
      env.compile(sf.body);
      env.end_regs();
      env.emit<ops::ScopeEnd>(f.pos);
    }

    FormImp *Sexpr::clone() const { return new Sexpr(body.begin(), body.end()); }

    void Sexpr::dump(ostream &out) const {
      out << '(';
      Body::dump(out);
      out << ')';
    }   

    void Sexpr::compile(Forms::const_iterator &in,
                        Forms::const_iterator end,
                        Env &env) const {
      env.compile((in++)->as<Sexpr>().body);
    }
    
    FormImp *Split::clone() const { return new Split(body.begin(), body.end()); }

    void Split::dump(ostream &out) const {
      out << '|';
      Body::dump(out);
    }   

    void Split::compile(Forms::const_iterator &in,
                        Forms::const_iterator end,
                        Env &env) const {
      auto &f(*in++);
      auto &sf(f.as<Split>());
      env.emit<ops::Split>(f.pos, sf.offs);
      env.compile(sf.body);
      env.emit<ops::SplitEnd>(f.pos);
    }

    FormImp *Stack::clone() const { return new Stack(body.begin(), body.end()); }

    void Stack::dump(ostream &out) const {
      out << '[';
      Body::dump(out);
      out << ']';
    } 

    void Stack::compile(Forms::const_iterator &in,
                        Forms::const_iterator end,
                        Env &env) const {
      auto &f(*in++);
      auto &b(f.as<Stack>().body);

      bool split(b.empty() ||
                 &b.front().type != &forms::Id::type ||
                 b.front().as<forms::Id>().id != env.sym(".."));
      
      if (split) { env.emit<ops::Split>(f.pos); }
      env.compile(split ? b.begin() : b.begin()+1, b.end());
      env.emit<ops::Stack>(f.pos, split);
    }
  }
}
