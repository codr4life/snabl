#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/libs/abc.hpp"
#include "snabl/std.hpp"
#include "snabl/timer.hpp"

namespace snabl {
  namespace libs {
    Abc::Abc(Env &env): Lib(env, env.sym("abc"), &env.s_lib) { }

    void Abc::init() {
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

      add_macro(env.sym("async:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  const auto form(*in++);                 
                  env.emit<ops::Async>(form.pos, 1);
                  
                  if (in == end) {
                    throw SyntaxError(form.pos, "Missing async body");
                  }
                  
                  env.compile(*in++);
                  env.emit<ops::Async>(form.pos, -1);
                });

      add_macro(env.sym("bench:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  const auto form(*in++);                 
                  auto &op(env.emit<ops::Bench>(form.pos));
                  
                  if (in == end) {
                    throw SyntaxError(form.pos, "Missing bench body");
                  }
                  
                  env.compile(*in++);
                  env.emit<ops::Stop>(form.pos);
                  op.end_pc = env.ops.size();
                }); 

      add_macro(env.sym("enum:"),
                [this](Forms::const_iterator &in,
                       Forms::const_iterator end,
                       Env &env) {
                  const auto form(*in++);
                  auto id((in++)->as<forms::Id>().id);
                  auto body(*in++);
                  
                  vector<Sym> alts;
                  for (auto &f: body.as<forms::Body>().body) {
                    alts.push_back(f.as<forms::Id>().id);
                  }

                  env.lib().add_enum_type(id, alts);
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

      add_macro(env.sym("is:"),
                [this](Forms::const_iterator &in,
                       Forms::const_iterator end,
                       Env &env) {
                  auto &form(*in++);                  

                  if (in == end) {
                    throw SyntaxError(form.pos, "Missing target type");
                  }

                  auto child(*in++);
                  const auto &child_id(child.as<forms::Id>().id);
                  auto child_type(env.lib().get_type(child_id));

                  if (!child_type) {
                    throw CompileError(form.pos,
                                       fmt("Target type not found: %0",
                                           {child_id}));                    
                  }
                                                     
                  if (in == end) {
                    throw SyntaxError(form.pos, "Missing parent types");
                  }
                  
                  auto parents(*in++);

                  auto derive = [&env, &form, &child_type](Sym parent_id) {
                    auto parent_type(env.lib().get_type(parent_id));
                    
                    if (!parent_type) {
                      throw CompileError(form.pos,
                                         fmt("Parent type not found: %0",
                                             {parent_id}));
                    }

                    if (!dynamic_cast<Trait *>(parent_type)) {
                      throw CompileError(form.pos, "Parent type is not a trait");
                    }

                    child_type->derive(*parent_type);
                  };
                  
                  if (&parents.type == &forms::Id::type) {
                    derive(parents.as<forms::Id>().id);
                  } else {
                    for (auto &id: parents.as<forms::Body>().body) {
                      derive(id.as<forms::Id>().id);
                    }
                  }
                }); 

      add_macro(env.sym("func:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &lib(env.lib());
                  const auto &form(*in++);
                  
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
                    throw SyntaxError(form.pos, "Missing let place");
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

      add_macro(env.sym("sync:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  const auto form(*in++);                 
                  env.emit<ops::Async>(form.pos, 1);
                  if (in == end) { throw SyntaxError(form.pos, "Missing sync body"); }
                  env.compile(*in++);
                  env.emit<ops::Async>(form.pos, -1);
                  env.emit<ops::Sync>(form.pos);
                });
      
      add_macro(env.sym("task:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &form(*in++);

                  if (in == end) {
                    throw SyntaxError(form.pos, "Missing task body");
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
                  auto &op(env.emit<ops::Try>(form.pos));
                  if (in == end) { throw SyntaxError(form.pos, "Missing try body"); }
                  env.compile(*in++);
                  env.emit<ops::TryEnd>(form.pos);
                  env.emit<ops::Push>(form.pos, env.nil_type);
                  op.end_pc = env.ops.size();
                });

      add_macro(env.sym("while:"),
                [](Forms::const_iterator &in,
                   Forms::const_iterator end,
                   Env &env) {
                  auto &form(*in++);               

                  if (in == end) {
                    throw SyntaxError(form.pos, "Missing while body");
                  }

                  const auto start_pc(env.ops.size());
                  env.compile(*in++);
                  env.emit<ops::If>(form.pos, start_pc);
                });; 
      
      add_fimp(env.sym("catch"),
               {Box(env.error_type)},
               [this](Fimp &fimp) {
                 env.push(env.pop().as<UserError>().val);
               });

      add_fimp(env.sym("is"),
               {Box(env.maybe_type), Box(env.meta_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.is(*y.as<AType *>()));
               });

      add_fimp(env.sym("="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.eqval(y));
               });

      add_fimp(env.sym("!="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, !x.eqval(y));
               });

      add_fimp(env.sym("=="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.equid(y));
               });

      add_fimp(env.sym("!=="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, !x.equid(y));
               });

      add_fimp(env.sym("<"),
               {Box(env.cmp_type), Box(env.cmp_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.cmp(y) == Cmp::LT);
               });

      add_fimp(env.sym(">"),
               {Box(env.cmp_type), Box(env.cmp_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 env.push(env.bool_type, x.cmp(y) == Cmp::GT);
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
               [this](Fimp &fimp) { env.push(env.bool_type, env.pop()); });

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
                   auto v(i->call());
                   if (v) { env.push(*v); }
                 }
               });

      add_fimp(env.sym("stack"),
               {Box(env.seq_type)},
               [this](Fimp &fimp) {
                 auto in(env.pop().iter());
                 auto out(StackPtr::make(&env.stack_type.pool));
                 drain(*in, *out);
                 env.push(env.stack_type, out);
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

      add_fimp(env.sym("sym"),
               {Box(env.enum_type)},
               [this](Fimp &fimp) {
                 env.push(env.sym_type, env.pop().as_enum.id);
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
                 env.push(env.i64_type, env.pop().as_time.as_ms());
               });
      
      add_fimp(env.sym("sleep"),
               {Box(env.time_type)},
               [this](Fimp &fimp) {
                 const Time time(env.pop().as_time);
                 this_thread::sleep_for(nanoseconds(time.ns));
               });

      add_fimp(env.sym("bin"),
               {Box(env.str_type)},
               [this](Fimp &fimp) {
                 auto &in(*env.pop().as<StrPtr>());
                 env.push(env.bin_type,
                          BinPtr::make(&env.bin_type.pool, in.begin(), in.end()));
               });

      add_fimp(env.sym("str"),
               {Box(env.bin_type)},
               [this](Fimp &fimp) {
                 auto &in(*env.pop().as<BinPtr>());
                 env.push(env.str_type,
                          StrPtr::make(&env.str_type.pool, in.begin(), in.end()));
               });

      add_fimp(env.sym("fopen"),
               {Box(env.str_type), Box(env.io_type)},
               [this](Fimp &fimp) {
                 auto m(env.pop().as_enum.id);
                 auto fn(env.pop().as<StrPtr>());

                 if (m == env.sym("r")) {
                   env.push(fopen(env, *fn, ios::in));
                 } else if (m == env.sym("w")) {
                   env.push(fopen(env, *fn, ios::out));
                 } else if (m == env.sym("rw")) {
                   env.push(fopen(env, *fn, ios::in | ios::out));
                 } else {
                   throw RuntimeError(env, fmt("Invalid mode: %0", {m}));
                 }
               });

      add_fimp(env.sym("str"),
               {Box(env.rfile_type)},
               [this](Fimp &fimp) {
                 auto fp(env.pop().as<FilePtr>());
                 
                 env.push(*async(env, [this, fp]() {
                       auto &f(*fp);
                       f.seekg(0, ios::end);
                       const auto size = f.tellg();
                       f.seekg(0, ios::beg);
                       vector<char> buf(size);
                       f.read(buf.data(), size);
                       return Box(env.str_type,
                                  StrPtr::make(&env.str_type.pool, buf.data(), size));
                     }));
               });

      add_fimp(env.sym("test="),
               {Box(env.maybe_type), Box(env.maybe_type)},
               [this](Fimp &fimp) {
                 Box y(env.pop()), x(env.pop());
                 
                 if (!x.eqval(y)) {
                   throw RuntimeError(env, fmt("Expected (%0), was (%1)", {y, x}));
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
