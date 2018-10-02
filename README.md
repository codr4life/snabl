![Logo](logo.png?raw=true)

### Features
The following is a list of what could be considered defining features.

#### Fixation
Snabl allows reordering functions and arguments within expressions. Expressions may be enclosed in ```()```, which evaluates the enclosed expression separately; and divided using ```,```, which evaluates the rest separately.

```
1 + 2 * 3

Compile error in row 1, col 6: Extra func: *
	 
1 +, 2 * 3

[7]
```

#### Concatenation
Much like Forth, Snabl supports directly manipulating the parameter stack. ```;``` calls the current function, ```--``` in this case, before evaluating the rest separately.

```
43 -- dup!

[43 42]

43 --; dup!

[42 42]
```

#### Namespacing
Snabl uses sigils to simplify local reasoning. Types start with uppcercase letters, variables are prefixed with `@`, macros use `!` or `:` as suffix depending on arity. Apart from literals; anything else is either a constant such as `t`, `f` and `nil`; or a function.

```
{41 let: foo @foo ++}

[42]
```

#### Typing
Snabl supports strong, first class Types. The root type ```T``` may be used to indicate any type except ```Nil```.

```
42 Int?

[t]

Int T?

[t]

42 Nil?

[f]

nil T?

[f]
```

#### Function
Snabl embraces but does not mandate functional programming. It supports first class functions, pattern matching, cheap lambdas and tail recursion. The following example implements a tail recursive fibonacci.

```
func: my-fib<Int Int Int> {
  let: (n a b)

  @n switch:,
    0? @a
    1? @b,
    --; @b dup! @a +; recall!
}
```

The same algorithm may be implemented entirely without variables by manipulating the stack directly.

```
func: my-fib<Int Int Int> (
  rswap! switch:,
    0? sdrop!
    1? drop!,
    --; rswap! dup! rot! +; recall!
)
```

#### Failure
Snabl offers two flavors of error handling, ```Maybe``` and ```throw```/```catch```. Any value may be passed as ```Maybe```, stray ```nil```'s are usually caught in the next call.

```
func: foo<T> _
foo 42

[42]

foo nil

[nil]
Error in row 1, col 0:
'Func not applicable: foo'

func: bar<Maybe> _
bar 42

[42]

bar nil

[nil]
```

Any value may be thrown. ```try:``` runs its argument and pushes an error or ```nil``` on the stack, ```catch``` may be used to retrieve the thrown value.

```
(try:, throw 41) catch; ++

[42]
```

#### Integration
Snabl integrates deeply into C++ and the STL, and is relatively trivial to embed.

Example 5
```
snabl::Env env;

env.home.add_fimp(
  env.sym("my-fib"),
  {snabl::Box(env.int_type)},
  [&env](snabl::Fimp &fimp) {
    snabl::Box &v(env.peek());
    snabl::Int n(v.as<snabl::Int>()), a(0), b(1);

    while (n-- > 1) {
      std::swap(a, b);
      b += a;
    }

    v.as<snabl::Int>() = b;
  });

env.run("say, my-fib 10");
```

#### Portability
Snabl requires a C++17-capable compiler and CMake to build.

```
$ git clone https://github.com/codr4life/snabl.git
$ cd snabl
$ mkdir build
$ cd build
$ cmake ..
$ sudo make install
$ snabl

Snabl v0.1

Press Return on empty line to eval.

1 + 2
 
[3]
```

### License
MIT