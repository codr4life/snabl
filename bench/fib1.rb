require_relative 'bench.rb'

def fib n
  n < 2 ? n : fib(n-1) + fib(n-2)
end

puts(bench(100) {fib 20})
