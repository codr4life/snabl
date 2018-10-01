from bench import bench

def test():
    for _ in range(10):
        try:
            raise 42
        except Exception as e:
            pass

print(int(bench(10000, 'test', '()')))
