func: fib<Int Int Int> Int (
  let: (n a b) _
  if: (zero? @n) @a, if: (one? @n) @b, @n --; @b dup @a +; recall
)

say, bench 10000 {fib 20 0 1; drop}; ms