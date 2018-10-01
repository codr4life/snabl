func: tail-fib<Int Int Int> (
	rswap!
 	switch:,
	  0? sdrop!
    1? drop!,
		--; rswap! dup! rot! +; recall!
)

say, (10000 bench:, tail-fib 20 0 1; drop!) ms