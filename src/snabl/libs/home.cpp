#include "snabl/call.hpp"
#include "snabl/env.hpp"
#include "snabl/libs/home.hpp"
#include "snabl/std.hpp"
#include "snabl/timer.hpp"

namespace snabl {
	namespace libs {
		Home::Home(Env &env): Lib(env, env.sym("home")) {
			env.no_type = add_type<Trait>(env.sym("_"));
			env.maybe_type = add_type<Trait>(env.sym("Maybe"));
			env.nil_type = add_type<NilType>(env.sym("Nil"), {env.maybe_type});
			env.a_type = add_type<Trait>(env.sym("A"), {env.maybe_type});
			env.num_type = add_type<Trait>(env.sym("Num"), {env.a_type});

			env.meta_type = add_type<MetaType>(env.sym("Type"), {env.a_type});	
			env.bool_type = add_type<BoolType>(env.sym("Bool"), {env.a_type});
			env.float_type = add_type<FloatType>(env.sym("Float"), {env.num_type});
			env.int_type = add_type<IntType>(env.sym("Int"), {env.num_type});
			env.sym_type = add_type<SymType>(env.sym("Sym"), {env.a_type});
			env.time_type = add_type<TimeType>(env.sym("Time"), {env.a_type});
			env.lambda_type = add_type<LambdaType>(env.sym("Lambda"), {env.a_type});
			
			add_macro(env.sym("t"), env.bool_type, true);			
			add_macro(env.sym("f"), env.bool_type, false);			
			add_macro(env.sym("nil"), env.nil_type, nullptr);			

			add_macro(env.sym("_"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									in++;
								});
			
			add_macro(env.sym("call"), ops::Call::type);
			add_macro(env.sym("ddrop"), ops::DDrop::type);

			add_macro(env.sym("drop"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									if (!env.ops.empty() &&
											&env.ops.back().type == &ops::Drop::type) {
										env.note(in->pos, "Rewriting (drop drop) as (ddrop)");
										env.ops.pop_back();
										env.emit(ops::DDrop::type, (in++)->pos);
									} else if (!env.ops.empty() &&
														 (&env.ops.back().type == &ops::Dup::type ||
															&env.ops.back().type == &ops::Get::type ||
															&env.ops.back().type == &ops::Lambda::type ||
															&env.ops.back().type == &ops::Push::type)) {
										env.note(in->pos, fmt("Rewriting (%0 drop) as ()",
																					{env.ops.back().type.id}));
										env.ops.pop_back();
										in++;
									} else if (!env.ops.empty() &&
														 &env.ops.back().type == &ops::Swap::type) {
										env.note(in->pos, "Rewriting (swap drop) as (sdrop)");
										env.ops.pop_back();
										env.emit(ops::SDrop::type, (in++)->pos);
									} else {
										env.emit(ops::Drop::type, (in++)->pos);
									}
								});

			add_macro(env.sym("dup"), ops::Dup::type);
			add_macro(env.sym("recall"), ops::Recall::type);
			add_macro(env.sym("rot"), ops::Rot::type);
			add_macro(env.sym("rswap"), ops::RSwap::type);
			add_macro(env.sym("sdrop"), ops::SDrop::type);

			add_macro(env.sym("swap"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									if (!env.ops.empty() &&
											&env.ops.back().type == &ops::Rot::type) {
										env.note(in->pos, "Rewriting (rot swap) as (rswap)");
										env.ops.pop_back();
										env.emit(ops::RSwap::type, (in++)->pos);
									} else {
										env.emit(ops::Swap::type, (in++)->pos);
									}
								});
			
			add_macro(env.sym("let:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									auto &p(*in++);

									if (&p.type != &forms::Id::type && &p.type != &forms::Sexpr::type) {
										throw SyntaxError(p.pos, "Invalid let: place");
									}

									if (in == end) { throw Error("Missing let: value"); }
									env.compile(in++, in+1);
									
									if (&p.type == &forms::Id::type) {
										env.emit(ops::Let::type, form.pos, p.as<forms::Id>().id);
									} else {
										auto &body(p.as<forms::Sexpr>().body);
										
										for (auto pp = body.rbegin(); pp != body.rend(); pp++) {
											env.emit(ops::Let::type, form.pos, pp->as<forms::Id>().id);
										}
									}
								});

			add_macro(env.sym("if:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									env.compile(in++, in+1);
									auto &else_skip(env.emit(ops::Else::type, form.pos));
									size_t start_pc(env.ops.size());								
									env.compile(in++, in+1);
									auto &if_skip(env.emit(ops::Skip::type, form.pos));
									else_skip.as<ops::Else>().nops = env.ops.size()-start_pc;
									start_pc = env.ops.size();
									env.compile(in++, in+1);
									if_skip.as<ops::Skip>().nops = env.ops.size()-start_pc;
								});	

			add_macro(env.sym("switch:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &form(*in++);
									if (in == end) { throw Error("Missing switch: value"); }
									env.compile(in++, in+1);
									vector<ops::Skip *> skips;
									
									while (in != end) {
										if (in+1 != end) { env.emit(ops::Dup::type, form.pos); }
										env.compile(in++, in+1);

										if (in != end) {
											auto &else_op(env.emit(ops::Else::type,
																							form.pos).as<ops::Else>());
											auto start_pc = env.ops.size();
											env.emit(ops::Drop::type, form.pos);
											env.compile(in++, in+1);

											if (in != end) {
												skips.push_back(&env.emit(ops::Skip::type,
																									form.pos,
																									env.ops.size()+1).as<ops::Skip>());
											}
											
											else_op.nops = env.ops.size()-start_pc;
										}
									}

									for (auto &s: skips) { s->nops = env.ops.size()-s->nops; }
								});	
			
			add_macro(env.sym("func:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Env &env) {
									auto &lib(env.lib());
									const auto &form(*in++);
									const auto &id_form((in++)->as<forms::Id>());

									auto &args_form(*in++);
									vector<Box> args;
									
									if (&args_form.type == &forms::TypeList::type) {
										auto &ids(args_form.as<forms::TypeList>().ids);

										for (const auto id: ids) {
											const auto t(lib.get_type(id));
											if (!t) { throw Error(fmt("Unknown type: %0", {id})); }
											args.emplace_back(*t);
										}
									} else {
										throw SyntaxError(args_form.pos,
																			fmt("Invalid func args: %0",
																					{args_form.type.id}));
									}

									auto &rets_form(*in++);
									vector<ATypePtr> rets;
									
									if (&rets_form.type == &forms::Id::type) {
										const auto id(rets_form.as<forms::Id>().id);
										const auto t(lib.get_type(id));
										if (!t) { throw Error(fmt("Unknown type: %0", {id})); }
										rets.push_back(*t);
									} else {
										throw SyntaxError(rets_form.pos,
																			fmt("Invalid func rets: %0",
																					{rets_form.type.id}));
									}

									auto fi = lib.add_fimp(id_form.id, args, rets, in++, in+1);
									Fimp::compile(fi, form.pos);
								});

			add_fimp(env.sym("throw"),
							 {Box(env.a_type)}, {},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 throw UserError(env, env.pc->pos, env.pop());
							 });

			add_fimp(env.sym("isa"),
							 {Box(env.meta_type), Box(env.meta_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.as<ATypePtr>()->isa(y.as<ATypePtr>()));
							 });

			add_fimp(env.sym("isa"),
							 {Box(env.maybe_type), Box(env.meta_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.type()->isa(y.as<ATypePtr>()));
							 });

			add_fimp(env.sym("="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.eqval(y));
							 });

			add_fimp(env.sym("=="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.equid(y));
							 });

			add_fimp(env.sym("<"),
							 {Box(env.a_type), Box(env.a_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Box y(env.pop()), x(env.pop());
								 env.push(env.bool_type, x.cmp(y) == Cmp::LT);
							 });
	
			add_fimp(env.sym("int"),
							 {Box(env.float_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 const Float v(env.pop().as<Float>());
								 env.push(env.int_type, Int(v));
							 });

			add_fimp(env.sym("z?"),
							 {Box(env.int_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.bool_type, !env.pop().as<Int>());
							 });

			add_fimp(env.sym("one?"),
							 {Box(env.int_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.bool_type, env.pop().as<Int>() == 1);
							 });
			
			add_fimp(env.sym("float"),
							 {Box(env.int_type)},
							 {env.float_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 const Int v(env.pop().as<Int>());
								 env.push(env.float_type, Float(v));
							 });

			add_fimp(env.sym("++"),
							 {Box(env.int_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.int_type, env.pop().as<Int>()+1);
							 });

			add_fimp(env.sym("--"),
							 {Box(env.int_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.int_type, env.pop().as<Int>()-1);
							 });
			
			add_fimp(env.sym("+"),
							 {Box(env.int_type), Box(env.int_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Int y(env.pop().as<Int>()), x(env.pop().as<Int>());
								 env.push(env.int_type, x+y);
							 });

			add_fimp(env.sym("-"),
							 {Box(env.int_type), Box(env.int_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Int y(env.pop().as<Int>()), x(env.pop().as<Int>());
								 env.push(env.int_type, x-y);
							 });
			
			add_fimp(env.sym("*"),
							 {Box(env.int_type), Box(env.int_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Int y(env.pop().as<Int>()), x(env.pop().as<Int>());
								 env.push(env.int_type, x*y);
							 });

			add_fimp(env.sym("bool"),
							 {Box(env.maybe_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.bool_type, env.pop().is_true());
							 });

			add_fimp(env.sym("dump"),
							 {Box(env.maybe_type)}, {},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.pop().dump(cerr);
								 cerr << endl;
							 });

			add_fimp(env.sym("say"),
							 {Box(env.maybe_type)}, {},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.pop().print(cout);
								 cout << endl;
							 });
			
			add_fimp(env.sym("ns"),
							 {Box(env.int_type)}, {env.time_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.time_type, env.pop().as<Int>());
							 });			

			add_fimp(env.sym("ms"),
							 {Box(env.int_type)}, {env.time_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.time_type, Time::ms(env.pop().as<Int>()));
							 });			

			add_fimp(env.sym("ms"),
							 {Box(env.time_type)}, {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 env.push(env.int_type, env.pop().as<Time>().as_ms());
							 });
			
			add_fimp(env.sym("sleep"),
							 {Box(env.time_type)}, {},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 const Time time(env.pop().as<Time>());
								 this_thread::sleep_for(nanoseconds(time.ns));
							 });

			add_fimp(env.sym("test="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 {},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 Box y(env.pop()), x(env.pop());
								 
								 if (!x.eqval(y)) {
									 throw Error(fmt("Expected (%0), was (%1)", {y, x}));
								 }
							 });

			add_fimp(env.sym("bench"),
							 {Box(env.int_type), Box(env.a_type)}, {env.time_type},
							 [](Call &call) {
								 Env &env(call.scope.env);
								 const Box target(env.pop());
								 const Int reps(env.pop().as<Int>());
								 for (int i(0); i < reps/2; i++) { target.call(true); }
								 
								 Timer t;
								 for (Int i(0); i < reps; i++) { target.call(true); }
								 env.push(env.time_type, t.ns());
							 });

			add_fimp(env.sym("fibonacci"),
							 {Box(env.int_type)}, {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope.env);								 
								 Int n(env.pop().as<Int>()), a(0), b(1);
								 
								 while (n-- > 1) {
									 swap(a, b);
									 b += a;
								 }

								 env.push(env.int_type, b);
							 });
		}
	}
}
