#include "snabl/env.hpp"
#include "snabl/form.hpp"
#include "snabl/run.hpp"

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
    const FormType<More> More::type("more");
    const FormType<Query> Query::type("query");
    const FormType<Ref> Ref::type("ref");
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
                [&lib, pos](Sym id) {
                  auto t(lib.get_type(id));
                  
                  if (!t) {
                    throw CompileError(pos, "Unknown type: " + id.name());
                  }
                  
                  return Box(*t);
                });
      
      auto fnp(lib.get_func(id));
      if (!fnp) { throw CompileError(pos, fmt("Unknown func: '%0'", {id.name()})); }
      auto &fn(*fnp);
      
      if (Int(args.size()) != fn->nargs) {
        throw CompileError(pos, fmt("Wrong number of args: %0", {fn->id}));
      }
      
      auto fip(fn->get_best_fimp(args.begin(), args.end()));
      auto &fi(*fip);
               
      if (!fi) { throw CompileError(pos, fmt("Unknown fimp: %0", {fn->id})); }
      if (!fi->imp) { snabl::Fimp::compile(fi, pos); }
      env.emit(ops::Funcall::type, pos, fi);
    }

    Id::Id(Sym id): id(id) { }

    FormImp *Id::clone() const { return new Id(id); }

    void Id::dump(ostream &out) const { out << id.name(); }
    
    void Id::compile(Forms::const_iterator &in,
                     Forms::const_iterator end,
                     Env &env) const {
      auto &form(*in);
      auto &id(form.as<Id>().id);

      if (id.name().front() == '@') {
        in++;
        env.emit(ops::Get::type, form.pos, env.sym(id.name().substr(1)));
      } else if (isupper(id.name().front())) {
        in++;
        auto t(env.lib().get_type(id));
        if (!t) { throw CompileError(form.pos, fmt("Unknown type: %0", {id})); }
        env.emit(ops::Push::type, form.pos, env.meta_type, *t);
      } else {
        auto &lib(env.lib());
        auto m(lib.get_macro(id));
        
        if (m) {
          (*m)->call(in, end, env);
        } else {
          in++;
          auto fn(lib.get_func(id));

          if (!fn) {
            throw CompileError(form.pos, fmt("Unknown id: '%0'", {id.name()}));
          }
          
          env.emit(ops::Funcall::type, form.pos, *fn);
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
      env.emit(ops::Push::type, form.pos, form.as<Lit>().val);
    }

    FormImp *More::clone() const { return new More(body.begin(), body.end()); }

    void More::dump(ostream &out) const {
      out << ", ";
      Body::dump(out);
    }   

    void More::compile(Forms::const_iterator &in,
                       Forms::const_iterator end,
                       Env &env) const {
      env.compile((in++)->as<More>().body);
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
        env.emit(ops::Eqval::type, qf.pos, qf.as<Lit>().val);
      } else if (&qf.type == &forms::Id::type) {
        if (isupper(qf.as<forms::Id>().id.name().front())) {
          auto &id(qf.as<forms::Id>().id);
          auto t(env.lib().get_type(id));
          if (!t) { throw CompileError(qf.pos, fmt("Unknown type: %0", {id})); }
          env.emit(ops::Isa::type, qf.pos, *t);
        } else {
          env.compile(qf);
          env.emit(ops::Eqval::type, qf.pos);
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
        auto &start_op(env.emit(ops::Lambda::type, f.pos, is_scope));
        auto &start(start_op.as<ops::Lambda>());
        if (is_scope) { env.begin_regs(); }
        env.compile(rf.as<Body>().body);
        if (is_scope) { env.end_regs(); }
        env.emit(ops::Return::type, f.pos);
        start.start_pc = start_op.next;
        start.end_pc = env.ops.size();
      } else {
        throw CompileError(rf.pos, fmt("Invalid ref: %0", {rf.type.id}));
      }
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
      env.emit(ops::Scope::type, f.pos);
      env.begin_regs();
      env.compile(sf.body);
      env.end_regs();
      env.emit(ops::ScopeEnd::type, f.pos);
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
      env.emit(ops::Split::type, f.pos, sf.offs);
      env.compile(sf.body);
      env.emit(ops::SplitEnd::type, f.pos);
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
      
      if (split) { env.emit(ops::Split::type, f.pos); }
      env.compile(split ? b.begin() : b.begin()+1, b.end());
      env.emit(ops::Stack::type, f.pos, split);
    }
  }
}
