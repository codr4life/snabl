#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/std.hpp"
#include "snabl/timer.hpp"

namespace snabl {
  namespace libs {
    Home::Home(Env &env): Lib(env, env.sym("home")) { }

    void Home::init() {
      add_macro(env.sym("t"), env.bool_type, true);     
      add_macro(env.sym("f"), env.bool_type, false);      
      add_macro(env.sym("nil"), env.nil_type);      

      add_macro<ops::Nop>(env.sym("_"));
      add_macro<ops::Call>(env.sym("call!"));
      add_macro<ops::DDrop>(env.sym("ddrop!"));
      add_macro<ops::Drop>(env.sym("drop!"));
      add_macro<ops::Dup>(env.sym("dup!"));
      add_macro<ops::Recall>(env.sym("recall!"));
      add_macro<ops::Return>(env.sym("return!"));
      add_macro<ops::Rot>(env.sym("rot!"));
      add_macro<ops::RSwap>(env.sym("rswap!"));
      add_macro<ops::SDrop>(env.sym("sdrop!"));
      add_macro<ops::Swap>(env.sym("swap!"));
      add_macro<ops::Sync>(env.sym("sync!"));
      add_macro<ops::Throw>(env.sym("throw!"));
      add_macro<ops::Yield>(env.sym("yield!"));

      add_macro(env.sym("bench:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  const auto form(*in++);                 
                  auto &op(env.emit<ops::Bench>(form.pos));
                  if (in == end) {
                    throw CompileError(form.pos, "Missing bench form");
                  }
                  env.compile(*in++);
                  env.emit<ops::Stop>(form.pos);
                  op.end_pc = env.ops.size();
                }); 

      add_macro(env.sym("if:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &form(*in++);
                  auto &else_skip(env.emit<ops::Else>(form.pos));
                  env.compile(*in++);
                  auto &if_skip(env.emit<ops::Jump>(form.pos));
                  else_skip.skip_pc = env.ops.size();
                  env.compile(*in++);
                  if_skip.end_pc = env.ops.size();
                }); 

      add_macro(env.sym("func:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &lib(env.lib());
                  const auto &form(*in++);
                  
                  if (&in->type != &forms::Fimp::type) {
                    throw SyntaxError(form.pos,
                                      fmt("Invalid func: %0",
                                          {in->type.id}));
                  }

                  auto &id_form((in++)->as<forms::Fimp>());
                  vector<Box> args;
                  
                  for (const auto id: id_form.type_ids) {
                    const auto t(lib.get_type(id));

                    if (!t) {
                      throw CompileError(form.pos, fmt("Unknown type: %0", {id}));
                    }
                    
                    args.emplace_back(*t);
                  }
                  
                  auto &fi = lib.add_fimp(id_form.id, args, *in++);
                  fi.compile(form.pos);
                });

      add_macro(env.sym("let:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &form(*in++);

                  if (in == end) {
                    throw CompileError(form.pos, "Missing let place");
                  }
                  
                  auto &p(*in++);

                  if (&p.type == &forms::Id::type) {
                    env.emit<ops::Let>(form.pos, p.as<forms::Id>().id);
                  } else {
                    auto &b(p.as<forms::Body>().body);

                    for (auto pp = b.rbegin(); pp != b.rend(); pp++) {
                      env.emit<ops::Let>(form.pos, pp->as<forms::Id>().id);
                    }
                  }
                });
            
      add_macro(env.sym("switch:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &form(*in++);

                  vector<ops::Jump *> skips;
                  auto &cases((in++)->as<forms::Body>());

                  for (auto f(cases.body.begin()); f != cases.body.end();) {
                    if (f+1 != cases.body.end()) { env.emit<ops::Dup>(form.pos); }
                    env.compile(*f++);

                    if (f != cases.body.end()) {
                      auto &else_op(env.emit<ops::Else>(form.pos));
                      env.emit<ops::Drop>(form.pos);
                      env.compile(*f++);
                      
                      if (f != cases.body.end()) {
                        skips.push_back(&env.emit<ops::Jump>(form.pos));
                      }

                      else_op.skip_pc = env.ops.size();
                    }
                  }

                  for (auto &s: skips) { s->end_pc = env.ops.size(); }
                }); 

      add_macro(env.sym("task:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &form(*in++);

                  if (in == end) {
                    throw CompileError(form.pos, "Missing task body");
                  }
                  
                  auto &op(env.emit<ops::Task>(form.pos));
                  
                  env.compile(*in++);
                  env.emit<ops::Stop>(form.pos);
                  op.end_pc = env.ops.size();
                }); 

      add_macro(env.sym("times:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &form(*in++);
                  const I64 i_reg(env.next_reg(form.pos));
                  env.emit<ops::Times>(form.pos, i_reg);
                  const auto start_pc(env.ops.size());

                  auto &jump(env.emit<ops::JumpIf>(form.pos, [&env, i_reg]() {
                        return !env.get_reg<I64>(i_reg)--;
                      }));
                  
                  env.compile(*in++);
                  env.emit<ops::Jump>(form.pos, start_pc);
                  jump.end_pc = env.ops.size();
                }); 

      add_macro(env.sym("try:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  const auto form(*in++);
                  auto &op(env.emit<ops::Try>(form.pos, env.next_reg(form.pos)));
                  if (in == end) { throw CompileError(form.pos, "Missing try body"); }
                  env.compile(*in++);
                  env.emit<ops::TryEnd>(form.pos, op.state_reg);
                  env.emit<ops::Push>(form.pos, env.nil_type);
                  op.end_pc = env.ops.size();
                });

      add_fimp(env.sym("catch"),
               {Box(env.error_type)},
               [this](Fimp &fimp) {
                 env.push(env.pop().as<UserError>().val);
               });

      add_fimp(env.sym("isa"),
               {Box(env.maybe_type), Box(env.meta_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.isa(*y.as<AType *>()));
               });

      add_fimp(env.sym("="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.eqval(y));
               });

      add_fimp(env.sym("=="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.equid(y));
               });

      add_fimp(env.sym("<"),
               {Box(env.root_type), Box(env.root_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.cmp(y) == Cmp::LT);
               });
  
      add_fimp(env.sym("int"),
               {Box(env.float_type)},
               [this](Fimp &fimp) {
                 const Float v(env.pop().as_float);
                 env.push(env.i64_type, I64(v));
               });

      add_fimp(env.sym("float"),
               {Box(env.i64_type)},
               [this](Fimp &fimp) {
                 const I64 v(env.pop().as_i64);
                 env.push(env.float_type, Float(v));
               });

      add_fimp(env.sym("++"),
               {Box(env.i64_type)},
               [this](Fimp &fimp) {
                 env.peek().as_i64++;
               });

      add_fimp(env.sym("--"),
               {Box(env.i64_type)},
               [this](Fimp &fimp) {
                 env.peek().as_i64--;
               });
      
      add_fimp(env.sym("+"),
               {Box(env.i64_type), Box(env.i64_type)},
               [this](Fimp &fimp) {
                 I64 y(env.pop().as_i64);
                 env.peek().as_i64 += y;
               });

      add_fimp(env.sym("-"),
               {Box(env.i64_type), Box(env.i64_type)},
               [this](Fimp &fimp) {
                 I64 y(env.pop().as_i64);
                 env.peek().as_i64 -= y;
               });
      
      add_fimp(env.sym("*"),
               {Box(env.i64_type), Box(env.i64_type)},
               [this](Fimp &fimp) {
                 I64 y(env.pop().as_i64);
                 env.peek().as_i64 *= y;
               });

      add_fimp(env.sym("bool"),
               {Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 env.push(env.bool_type, env.pop().val_as_bool());
               });

      add_fimp(env.sym("push"),
               {Box(env.sink_type), Box(env.root_type)},
               [this](Fimp &fimp) {
                 auto v(env.pop());
                 env.pop().push(v);
               });
      
      add_fimp(env.sym("peek"),
               {Box(env.source_type)},
               [this](Fimp &fimp) {
                 auto v(env.pop().peek());
                 env.push(v ? *v : Box(env.nil_type));
               });

      add_fimp(env.sym("pop"),
               {Box(env.source_type)},
               [this](Fimp &fimp) {
                 env.pop().pop();
               });

      add_fimp(env.sym("iter"),
               {Box(env.seq_type)},
               [this](Fimp &fimp) {
                 env.push(env.iter_type, env.pop().iter());
               });

      add_fimp(env.sym(".."),
               {Box(env.seq_type)},
               [this](Fimp &fimp) {
                 auto i(env.pop().iter());

                 while (!i->is_done) {
                   auto v(i->call(env));
                   if (v) { env.push(*v); }
                 }
               });
      
      add_fimp(env.sym("dump"),
               {Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 env.pop().dump(cerr);
                 cerr << endl;
               });

      add_fimp(env.sym("say"),
               {Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 env.pop().print(cout);
                 cout << endl;
               });
      
      add_fimp(env.sym("len"),
               {Box(env.str_type)},
               [this](Fimp &fimp) {
                 env.push(env.i64_type, env.pop().as<StrPtr>()->size());
               });

      add_fimp(env.sym("len"),
               {Box(env.stack_type)},
               [this](Fimp &fimp) {
                 env.push(env.i64_type, env.pop().as<StackPtr>()->size());
               });

      add_fimp(env.sym("ns"),
               {Box(env.i64_type)},
               [this](Fimp &fimp) {
                 env.push(env.time_type, env.pop().as_i64);
               });      

      add_fimp(env.sym("ms"),
               {Box(env.i64_type)},
               [this](Fimp &fimp) {
                 env.push(env.time_type, Time::ms(env.pop().as_i64));
               });      

      add_fimp(env.sym("ms"),
               {Box(env.time_type)},
               [this](Fimp &fimp) {
                 env.push(env.i64_type, env.pop().as<Time>().as_ms());
               });
      
      add_fimp(env.sym("sleep"),
               {Box(env.time_type)},
               [this](Fimp &fimp) {
                 const Time time(env.pop().as<Time>());
                 this_thread::sleep_for(nanoseconds(time.ns));
               });

      add_fimp(env.sym("fopen"),
               {Box(env.str_type)},
               [this](Fimp &fimp) {
                 auto fn(env.pop().as<StrPtr>());
                 env.push(env.async_type, fopen(env, env.call().pos, *fn, ios::in));
               });

      add_fimp(env.sym("slurp"),
               {Box(env.rfile_type)},
               [this](Fimp &fimp) {
                 auto fp(env.pop().as<FilePtr>());
                 
                 env.push(env.async_type, async([this, fp]() {
                       auto &f(*fp);
                       f.seekg(0, ios::end);
                       const auto size = f.tellg();
                       f.seekg(0, ios::beg);
                       vector<char> buf(size);
                       f.read(buf.data(), size);
                       return Box(env.str_type, make_shared<Str>(buf.data(), size));
                     }));
               });

      add_fimp(env.sym("test="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 
                 if (!x.eqval(y)) {
                   throw RuntimeError(env,
                                      env.call().pos,
                                      fmt("Expected (%0), was (%1)", {y, x}));
                 }
               });

      add_fimp(env.sym("fib"),
               {Box(env.i64_type)},
               [this](Fimp &fimp) {
                 Box &v(env.peek());
                 I64 n(v.as_i64), a(0), b(1);
                 
                 while (n-- > 1) {
                   swap(a, b);
                   b += a;
                 }

                 v.as_i64 = b;
               });
    }
  }
}
