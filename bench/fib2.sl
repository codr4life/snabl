func: tail-fib<Int Int Int> (
	rswap!
 	switch: _,
	  0? sdrop!
    1? drop!,
		--; rswap! dup! rot! +; recall!
)

say, bench 10000 {tail-fib 20 0 1; drop!}; ms