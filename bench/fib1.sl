func: fib<Int> Int (
  let: n _			
  if: (@n < 2) @n, (fib, @n - 1) + (fib, @n - 2)
)

say, ms, bench 100 {(fib 20) drop}