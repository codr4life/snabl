func: tail-fib<Int Int Int> (
	rswap!
 	switch: (
	  0? sdrop!
    1? drop!,
		-- rswap! dup! rot! + recall!
  )
)

10000 bench: (20 0 1 tail-fib drop!) ms say