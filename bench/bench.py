from timeit import Timer

setup = 'gc.enable(); from __main__ import test'

def bench(reps):
    Timer('test()', setup).timeit(reps)
    return Timer('test()', setup).timeit(reps)*1000
