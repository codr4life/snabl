func: fib<Int Int Int> Int (
  let: (n a b) _
  switch: @n z? @a one? @b, --; @b dup @a +; recall
)

say, bench 10000 {fib 20 0 1; drop}; ms