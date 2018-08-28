#include "snabl/env.hpp"
#include "snabl/libs/home.hpp"

namespace snabl {
	namespace libs {
		Home::Home(Env &env): Lib(env, env.sym("home")) {
			env.maybe_type = add_type<Trait>(env.sym("Maybe"));
			env.a_type = add_type<Trait>(env.sym("A"), {env.maybe_type});
			env.no_type = add_type<Trait>(env.sym("_"));
			env.num_type = add_type<Trait>(env.sym("Num"), {env.a_type});
			env.bool_type = add_type<BoolType>(env.sym("Bool"), {env.a_type});
			env.float_type = add_type<FloatType>(env.sym("Float"), {env.num_type});
			env.int_type = add_type<IntType>(env.sym("Int"), {env.num_type});
	
			add_macro(env.sym("t"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) {
									out.emplace_back(ops::Push::type,
																	 (in++)->pos,
																	 Box(out.env.bool_type, true));			
								});

			add_macro(env.sym("f"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) {
									out.emplace_back(ops::Push::type,
																	 (in++)->pos,
																	 Box(out.env.bool_type, false));			
								});

			add_macro(env.sym("drop"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) {
									auto &form(*in++);
									out.emplace_back(ops::Drop::type, form.pos);			
								});

			add_macro(env.sym("let:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) {
									auto &form(*in++);
									auto &p(*in++);

									if (&p.type != &forms::Id::type) {
										throw SyntaxError(p.pos, "Invalid let: place");
									}
												 
									in->imp->compile(in, end, func, fimp, out);
									out.emplace_back(ops::PutVar::type,
																	 form.pos,
																	 p.as<forms::Id>().id);
								});

			add_macro(env.sym("if:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) {
									auto &form(*in++);
									out.compile(in++, in+1);
									auto &else_skip(out.emplace_back(ops::Else::type,
																									 form.pos,
																									 0));
									size_t start_pc(out.ops.size());								
									out.compile(in++, in+1);
									auto &if_skip(out.emplace_back(ops::Skip::type, form.pos, 0));
									else_skip.as<ops::Else>().nops = out.ops.size()-start_pc;
									start_pc = out.ops.size();
									out.compile(in++, in+1);
									if_skip.as<ops::Skip>().nops = out.ops.size()-start_pc;
								});	

			add_macro(env.sym("func:"),
								[](Forms::const_iterator &in,
									 Forms::const_iterator end,
									 FuncPtr &func, FimpPtr &fimp,
									 Bin &out) {
									auto &lib(out.env.lib());
									const auto &form(*in++);
									const auto &id_form((in++)->as<forms::Id>());

									auto &args_form(*in++);
									std::vector<Box> args;
									
									if (&args_form.type == &forms::TypeList::type) {
										auto &ids(args_form.as<forms::TypeList>().ids);

										for (const auto id: ids) {
											const auto t(lib.get_type(id));
											if (!t) { throw Error(fmt("Unknown type: %0", {id})); }
											args.emplace_back(t);
										}
									} else {
										throw SyntaxError(args_form.pos,
																			fmt("Invalid func args: %0",
																					{args_form.type.id}));
									}

									auto &rets_form(*in++);
									std::vector<ATypePtr> rets;
									
									if (&rets_form.type == &forms::Id::type) {
										const auto id(rets_form.as<forms::Id>().id);
										const auto t(lib.get_type(id));
										if (!t) { throw Error(fmt("Unknown type: %0", {id})); }
										rets.push_back(t);
									} else {
										throw SyntaxError(rets_form.pos,
																			fmt("Invalid func rets: %0",
																					{rets_form.type.id}));
									}
									
									auto fi = lib.add_fimp(id_form.id, args, rets, in, end);
									out.emplace_back(ops::Fimp::type, form.pos, fi);
									in = end;
								});

			add_fimp(env.sym("="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope->env);
			
								 Box
									 y(env.pop_stack()),
									 x(env.pop_stack());
			
								 env.push_stack(env.bool_type, x.is_eqval(y));
							 });

			add_fimp(env.sym("=="),
							 {Box(env.maybe_type), Box(env.maybe_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope->env);
			
								 Box
									 y(env.pop_stack()),
									 x(env.pop_stack());
			
								 env.push_stack(env.bool_type, x.is_equid(y));
							 });

			add_fimp(env.sym("<"),
							 {Box(env.a_type), Box(env.a_type)},
							 {env.bool_type},
							 [](Call &call) {
								 Env &env(call.scope->env);
			
								 Box
									 y(env.pop_stack()),
									 x(env.pop_stack());

								 env.push_stack(env.bool_type, x.cmp(y) == Cmp::LT);
							 });
	
			add_fimp(env.sym("int"),
							 {Box(env.float_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope->env);
								 const Float v(env.pop_stack().as<Float>());
								 env.push_stack(env.int_type, Int(v));
							 });

			add_fimp(env.sym("float"),
							 {Box(env.int_type)},
							 {env.float_type},
							 [](Call &call) {
								 Env &env(call.scope->env);
								 const Int v(env.pop_stack().as<Int>());
								 env.push_stack(env.float_type, Float(v));
							 });

			add_fimp(env.sym("+"),
							 {Box(env.int_type), Box(env.int_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope->env);
			
								 Int
									 y(env.pop_stack().as<Int>()),
									 x(env.pop_stack().as<Int>());
			
								 env.push_stack(env.int_type, x+y);
							 });

			add_fimp(env.sym("*"),
							 {Box(env.int_type), Box(env.int_type)},
							 {env.int_type},
							 [](Call &call) {
								 Env &env(call.scope->env);
			
								 Int
									 y(env.pop_stack().as<Int>()),
									 x(env.pop_stack().as<Int>());
			
								 env.push_stack(env.int_type, x*y);
							 });
		}
	}
}
