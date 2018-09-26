func: tail-fib<Int Int Int> (
  let: (n a b)
  @n switch:, 0? @a 1? @b, --; @b dup! @a +; recall!
)

say, bench 10000 {tail-fib 20 0 1; drop!}; ms