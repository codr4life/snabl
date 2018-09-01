func: fib<Int> Int (
  let: n _			
  if: (@n < 2) @n, (fib, @n --) + (fib, @n - 2)
)

say, bench 100 {fib 20; drop}; ms