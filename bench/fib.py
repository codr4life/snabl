from bench import bench

def fib(n):
    return n if n < 2 else fib(n-1) + fib(n-2)

print(int(bench(100, 'fib', '(20)')))
