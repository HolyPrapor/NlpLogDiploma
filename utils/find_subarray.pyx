#cython: boundscheck=False
#cython: wraparound=False
#cython: cdivision=True
cimport cython
cpdef int find_subarray(int[:] a, int[:] b):
    cdef int i, j
    cdef bint result
    for i in range(len(a)):
        result = True
        for j in range(len(b)):
            result = result and (a[i+j] == b[j])
            if not result:
                break
        if result:
            return i
    return -1

