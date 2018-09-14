from bench import bench

def fib(n, a, b):
    return a if n == 0 else b if n == 1 else fib(n-1, b, a+b)

print(int(bench(10000, 'fib', '(20, 0, 1)')))
