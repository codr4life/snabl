func: naive-fib<Int> (
  let: n _			
  if: (@n < 2) @n, (naive-fib, @n --) + (naive-fib, @n - 2)
)

say, bench 100 {naive-fib 20; drop}; ms