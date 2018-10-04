42 Int isa t test=
Int Num? t test=

42 nil? f test=
nil nil? t test=
nil bool f test=

t bool t test=
f bool f test=

0 bool f test=
1 bool t test=

1 1 = t test=
1 3 = f test=

1 1? t test=
1 3? f test=

'foo 'foo? t test=
'foo 'bar? f test=

1 3 < t test=
1 1 < f test=
3 1 < f test=

3.14 int 3 test=

1 3 + 4 test=
1 3 +<_ Int> 4 test=

''foo'bar'' ''foo'bar'' test=

''abc'' iter call! #a test=

1 3 5 drop! + 4 test=
21 dup! + 42 test=
1 3 swap! - 2 test=
1 2 3 rot! - - 4 test=

1 2 [3 4 5] len 3 test=
|1 2 [..3 4 5] len 5 test=
|3.. [..] len 3 test=

{42 let: foo @foo} 42 test=
{21 let: foo &{@foo}} call! 21 test=

t if: 1 3 1 test=
f if: 1 3 3 test=

7 switch: (, 3 -) 4 test=

35 switch: (
  (42 <) 'foo,
	drop! 'bar
) 'foo test=

35 switch: (
  (7 <) 'foo,
	drop! 'bar
) 'bar test=

35 switch: (
  (7 <) 'foo
	(42 <) 'bar,
	drop! 'baz
) 'bar test= 

2 3 times: ++ 5 test=
&(1 2 +) call! 3 test=

func: double<T> (2 *)
21 double 42 test=

42 let: result
func: closure<> @result
closure 42 test=

func: early<> ('before return! 'after)
early 'before test=

try: 42 nil test=
try: 42 drop! 42 test=

try: (41 throw) catch ++ 42 test=
try: (try: (41 throw) throw) catch ++ 42 test=

3 iter
dup! call! swap! dup! call! swap!
call! + + 3 test=