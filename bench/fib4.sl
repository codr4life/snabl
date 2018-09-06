func: fib<Int Int Int> Int (
	rswap
 	switch: _
	  z? sdrop
    one? drop,
		--; rswap dup rot +; recall
)

say, bench 10000 {fib 20 0 1; drop}; ms