from timeit import Timer

def bench(reps, target, args):
    fn = '{0}{1}'.format(target, args)
    
    return Timer(fn,
                 'gc.enable(); from __main__ import {0}; {1}'.format(target, fn)) \
                 .timeit(reps) * 1000

