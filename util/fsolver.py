import os, sys
from cffi import FFI
import numpy as np
import ctypes
## Compile command
'''
# OSX
g++ -fPIC \
    fsolver.cpp \
    -std=c++11 \
    -dynamiclib -o fsolver.dylib \
    -g -O3 -Wall -Wshadow -Wno-sign-compare
'''

ffi = FFI()
ffi.cdef(
"""
typedef float real_t;
typedef int index_t;
int solveLinear(float c1, float c0, float* s);
int solveQuadric(float c2, float c1, float c0, float* s);
int solveCubic(float c3, float c2, float c1, float c0, float* s);
"""
)
#
index_t = ctypes.c_int
real_t = ctypes.c_float

def platform_shared_library_suffix():
    import sys
    result = '.so'
    if 'win' in sys.platform.lower(): result = '.dll'
    ## No else if, because we want darwin to override win (which is a substring of darwin)
    if 'darwin' in sys.platform.lower(): result = '.dylib'
    if 'linux' in sys.platform.lower(): result = '.so'
    return result

# to deal with the problem met in linux
if 'darwin' in sys.platform.lower():
    libsolver = ffi.dlopen( os.path.join( os.path.dirname( __file__ ), 'fsolver' + platform_shared_library_suffix() ) )
if 'linux' in sys.platform.lower():
    libsolver = ffi.dlopen( os.path.join( os.path.dirname( __file__ ), 'fsolver_lib' + platform_shared_library_suffix() ) )

class fSolverError(Exception) : pass

def linear(a, b):
    s = np.zeros((1, ), dtype=real_t)
    ## update the function signature in c++ files
    num = libsolver.solveLinear(a, b, ffi.cast('real_t*', s.ctypes.data))
    if num < 0:
        raise fSolverError('solveLinear reported an error')
    # print(s.tolist())
    return [s[i] for i in xrange(num)]
    # return s.tolist()
def quadric(a, b, c):
    s = np.zeros((2, ), dtype=real_t)
    num = libsolver.solveQuadric(a, b, c, ffi.cast('real_t*', s.ctypes.data))
    if num < 0:
        raise fSolverError('solveQuadric reported an error')
    return [s[i] for i in xrange(num)]
    # return s.tolist()
def cubic(a, b, c, d):
    s = np.zeros((3, ), dtype=real_t)
    num = libsolver.solveCubic(a, b, c, d, ffi.cast('real_t*', s.ctypes.data))
    if num < 0:
        raise fSolverError('solveCubic reported an error')
    # print(s.shape)
    return [s[i] for i in xrange(num)]
    # return s.tolist()

if __name__ == '__main__':
    from random import randint
    import time

    print cubic(2,-4,-22, 24) # [4, -3, 1]
    print cubic(3,-10,14, 27) # [-1]
    print cubic(1, 6, 12,  8) # [-2]
    #
    eps = 1e-9
    ts = time.time()
    for i in xrange(1000000):
        a,b,c,d = randint(0,100),randint(0,100),randint(0,100),randint(0,100)
        for x in linear(a, b):
            if abs(a*x + b) > eps: print 'l',
        for x in quadric(a, b, c):
            if abs(a*x**2 + b*x + c) > eps: print 'q',
        for x in cubic(a, b, c, d):
            if abs(a*x**3 + b*x**2 + c*x + d) > eps: print 'c',
    print time.time() - ts