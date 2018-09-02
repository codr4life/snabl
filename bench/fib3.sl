func: fib<Int Int Int> Int (
  let: (n a b) _
  if: (@n zero?) @a (if: (@n one?) @b (@n --; @b @a + @b; recall))
)

say, bench 10000 {fib 20 0 1; drop}; ms