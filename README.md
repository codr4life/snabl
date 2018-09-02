![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered defining features.

#### Multiple Dispatch
Snabl supports dispatching functions on multiple arguments. Each function name may have multiple implementations, as long as they have the same number of arguments with different types.

#### Gradual Typing
Snabl allows but doesn't require specifying types of function arguments and variables.

#### Fixation
Snabl allows reordering functions/operators and arguments within expressions to fit the problem being solved. Expressions may be divided using ```,``` which evaluates the rest of the expression separately as an argument; enclosed in ```()```, which evaluates the enclosed expression the same way; and divided using ```;```, which calls the function before evaluating the rest separately. 

```
func: fib<Int> Int (
  let: n _			
  if: (@n < 2) @n, (fib, @n --) + (fib, @n - 2)
)
```

#### Concatenation
Much like Forth, Snabl supports directly manipulating the parameter stack from user code. The following code currently runs around 60% faster than the preceding, mostly due to not requiring local variables.

```
func: fib<Int> Int (
  dup if: (< 2) _, (fib, --; dup) swap + (fib, --)
)
```

#### Function
Snabl embraces but does not mandate functional programming. It supports first class functions, pattern matching, closures and tail recursion.

#### Reference Counting
Snabl uses reference counting instead of garbage collection, which leads to more predictable performance and resource usage.

#### Integration
Snabl integrates deeply into C++ and the STL, and is relatively trivial to embed.

```
snabl::Env env;

env.home.add_fimp(
  env.sym("fibonacci"),
  {snabl::Box(env.int_type)}, {env.int_type}, [](snabl::Call &call) {
    snabl::Env &env(call.scope.env);								 
    snabl::Int n(env.pop().as<snabl::Int>()), a(0), b(1);

    while (n-- > 1) {
      std::swap(a, b);
      b += a;
    }

    env.push(env.int_type, b);
  });
```

#### Portability
Besides a decent C++17 compiler and CMake, Snabl has no dependencies.

```
$ git clone https://github.com/codr4life/snabl.git
$ cd snabl
$ mkdir build
$ cd build
$ cmake ..
$ sudo make install
$ snabl
```

### License
MIT