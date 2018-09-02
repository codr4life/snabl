func: fib<Int Int Int> Int (
	let: a swap
  swap dup
	if: zero?
	  (drop drop @a)
		(dup if: one? drop (--; swap dup @a +; recall))
)

say, bench 10000 {fib 20 0 1; drop}; ms