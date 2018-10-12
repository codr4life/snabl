![Logo](logo.png?raw=true)

### Concatenation
Like Forth, Snabl supports manipulating the parameter stack.

```
1 2 3 swap! 

[1 3 2]

dup!

[1 3 2 2]

drop!

[1 3 2]
```

### Namespacing
Snabl uses sigils to simplify local reasoning. Types start with uppcercase letters, variables are prefixed with `@`, macros use `!` or `:` as suffix. Apart from literals; anything else is either a constant such as `t`, `f` and `nil`; or a function.

```
{41 let: foo @foo ++}

[42]
```

### Typing
Snabl supports strong, static, first class types. The root type ```T``` may refer to anything except ```nil```.

```
42 I64?

[t]

I64 T?

[t]

42 Nil?

[f]

nil T?

[f]
```

### Function
Snabl embraces but does not mandate functional programming. It supports first class functions, pattern matching, lambdas and tail recursion. The following example implements a tail recursive fibonacci.

```
func: my-fib<I64 I64 I64> {
  let: (n a b)

  @n switch:,
    0? @a
    1? @b,
    -- @b dup! @a + recall!
} 20 0 1 my-fib

[6765]
```

The same algorithm may be implemented anonymously in a single scope using a lambda with stack manipulation.

```
20 0 1 &(
  rswap! switch:,
    0? sdrop!
    1? drop!,
    -- rswap! dup! rot! + recall!
) call!

[6765]
```

### Multitasking
Snabl supports multitasking in the form of first class green threads. By default, code runs in the main task. New tasks may be started using ```task:```.

```
task: (3 times:, 'pong say yield!) drop!
3 times:, 'ping say yield!

ping
pong
ping
pong
ping
pong
[]
```

### Synchronicity
Operations that might hang return async actions, `sync!` yields the current task until the action is done and pushes any result.

foo.txt:
```
foo bar baz
```

```
''foo.txt'' r fopen

[(Async 0x125b850)]

sync! str sync!

[''foo bar baz'']
```

### Failure
Snabl offers two flavors of error handling, ```Maybe``` and ```throw!```/```catch```. Any value may be passed as ```Maybe```, stray ```nil```'s are usually caught in the next function call.

```
func: foo<T> _
foo 42

[42]

nil foo

[nil]
Error in row 1, col 0:
'Func not applicable: foo'

func: bar<Maybe> _
42 bar

[42]

nil bar

[nil]
```

Any value may be thrown. ```try:``` runs its argument and pushes an error or ```nil``` on the stack, ```catch``` may be used to retrieve the thrown value.

```
try: (41 throw!)
catch ++

[42]
```

### Integration
Snabl integrates deeply into C++ and the STL, and is relatively trivial to embed.

Example 5
```
snabl::Env env;

env.home.add_fimp(
  env.sym("my-fib"),
  {snabl::Box(env.int_type)},
  [&env](snabl::Fimp &fimp) {
    snabl::Box &v(env.peek());
    snabl::I64 n(v.as_i64), a(0), b(1);

    while (n-- > 1) {
      std::swap(a, b);
      b += a;
    }

    v.as_i64 = b;
  });

env.run("10 my-fib say");
```

### Portability
Snabl requires a C++17-capable compiler and CMake to build.

```
$ git clone https://github.com/codr4life/snabl.git
$ cd snabl
$ mkdir build
$ cd build
$ cmake ..
$ sudo make install
$ snabl

Snabl v0.3.1

Press Return on empty line to eval.

1 2 +
 
[3]
```

### License
MIT