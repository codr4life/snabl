(test=, 42 isa Int; t)
(test=, Int Num? t)

(test=, 42 nil? f)
(test=, nil nil? t)
(test=, nil bool; f)

(test=, t bool; t)
(test=, f bool; f)

(test=, 0 bool; f)
(test=, 1 bool; t)

(test=, 1 = 1; t)
(test=, 1 = 3; f)

(test= 1 1? t)
(test= 1 3? f)

(test= $foo $foo? t)
(test= $foo $bar? f)

(test=, 1 < 3; t)
(test=, 3 < 1; f)

(test=, 3.14 int; 3)

(test=, 1 + 3; 4)
(test= (1 +, 3 * 2) 7)
(test= (1 + (3 * 2)) 7)
(test= (1 +, 5 - 2) 4)

(test=, 1 3 5 drop +; 4)
(test=, 21 dup +; 42)
(test=, 1 3 swap -; 2)
(test= (1 2 3 rot -; -) 4)

(let: foo 42)
(test=  @foo 42)
(test=, {@foo} call 42)

(test=, 1 +<_ Int> 3; 4)

(test= if: (1 < 3) 5 7 5)
(test= if: (3 < 1) 5 7 7)

(test= (switch: 7, (drop t) $foo) $foo)
(test= (switch: 35, (< 42) $foo, drop $bar) $foo)
(test= (switch: 35, (< 7) $foo, drop $bar) $bar)
(test= (switch: 35, (< 7) $foo (< 42) $bar, drop $baz) $bar) 

func: foo1<A> A (* 2)
(test=, 21 foo1; 42)

(test= (try: (drop 7) 42 -) 35)
(test= (try: (catch; ++), throw 41) 42)
(test= (try: (catch; ++), try: throw, throw 41) 42)