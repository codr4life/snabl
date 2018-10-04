from bench import bench

def test():
    def task():
        for _ in range(100): yield
        
    t1, t2, t3 = task(), task(), task()
    
    for _ in range(100):
        next(t1)
        next(t2)
        next(t3)

print(int(bench(10000)))
