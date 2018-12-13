# Constant time memcmp() function

``` C
int cst_time_memcmp(const void *m1, const void *m2, size_t n)
```

The `cst_time_memcmp()` function return the result of a lexicographical 
comparison of the byte array `m1` and `m2`. The processing time is independed
of the byte values of `m1` and `m2`. It varies only with `n`. 

The `cst_time_memcmp()` function returns `0` when the `n` first bytes of `m1` and 
`m2` are equal. When the first different byte is found at index `k`, with `k < 0`, 
the function returns `-1` when `m1[k] < m2[k]`, and `+1` when `m1[k] > m2[k]`. 
Appart from the comparision result, this function reveals nothing of the byte 
values in arrays `m1` and `m2`.

The function returns also `0` when at least one of the following conditions is true.

- `n` is zero ;
- `m1` and `m2` are the same memory area ;
- `m1` is NULL ;
- `m2` is NULL.

## Table of content

  * [Algorithm](#algorithm)
  * [Alternate instructions](#alternate-instructions)
  * [The code](#the-code)
    * [Fastest implementation using subscipt](#fastest-implementation-using-subscipt)
    * [Fastest implementation using pointers](#fastest-implementation-using-pointers)
    * [Safest implementation using subscript](#safest-implementation-using-subscript)
    * [Safest implementation using pointers](#safest-implementation-using-pointers)
    * [Reference code from NetBSD](#reference-code-from-netbsd)
  * [Verification](#verification)
    * [Discussion](#discussion)
    * [Output of the program](#output-of-the-program)

## Algorithm

The following code illustrate the none constant time memcmp() algoritm's logic. 

``` C
int memcmp(const unsigned char *m1, const unsigned char *m1, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i ) {
        int diff = m1[i] - m2[i];
        if (diff != 0)
            return (diff < 0) ? -1 : +1;
    }
    return 0;
}
```

The constant time memcmp() function's algorithm is illustrated below

``` C
int cst_time_memcmp(const unsigned char *m1, const unsigned char *m1, size_t n) {
    int res = 0, diff;
    if (n > 0) {
        do {
            --n;
            diff = m1[n] - m2[n];
            if (diff != 0)
                res = diff;
        } while (n != 0);
    }
    return (res > 0) - (res < 0);
}
```

The above code compares the bytes from the last to the first byte.
Every time different bytes are found, the difference is stored in res.
Res contains thus the difference of the last different bytes met.
The function result is the same as the standard memcmp() function.
The difference is that all bytes have been compared.

For a constant time memcmp() function we can't use conditional 
branching instructions because the processing time will change
with the branch followed.

For this reason the if instruction in the loop is replaced with
a constant time instruction yielding the same result. 

``` C
if (diff != 0)
    res = diff;
```

is replaced with

``` C
res = (res & -!diff) | diff;
```

In the above instruction, `-!diff` is -1 (0xFFFFFFFF) when `diff == 0`
and 0 otherwise.
    
## Alternate instructions

The expression `!diff` could be compiled into machine code performing a branch. 
It depends on the compiler smartness and the machine code instruction set. 
Some processors have a very limited machine instruction set and compilers are
constrained to translate `!diff` into machine code using a branching 
instruction. This is not the case with x86 processors and good compilers.

When in doubt, or to provide safe portable code, the instruction

``` C
res = (res & -!diff) | diff;
```
    
must be replaced with

``` C
res = (res & (((diff - 1) & ~diff) >> 8)) | diff;
```
    
The function will be slightly slower but wont need a branching.

Another solution, shown below, use a table to implement the function.
But processing speed could be affected by partial caching of 
the table and thus indirectly reveal something of `m1` and `m2`
comparison. Use of this method is thus discouraged.

``` C
static signed char tbl[256] = {-1, 0, ... 0 };
res = (res & tbl[(unsigned char)diff]) | diff;
```

The generation of the return value from `res` could also be 
compiled into machine code using branching instructions. To 
avoid this in order to obtain portable code, the 
instruction

``` C
return (res > 0) - (res < 0);
```

must be replaced with

``` C
return ((res - 1) >> 8) + (res >> 8) + 1;
```
    
In the above expression, `res >> 8` is -1 when `res < 0` and `(res - 1) >> 8` is
-1 when `res <= 0`.

## The code

As a summary here are the different version on the code. See below in the 
program output to see execution time comparision. You may test your 
own configuration. 


### Fastest implementation using subscipt

``` C   
int cst_time_memcmp_fastest1(const void *m1, const void *m2, size_t n) 
{
    const unsigned char *pm1 = (const unsigned char*)m1; 
    const unsigned char *pm2 = (const unsigned char*)m2; 
    int res = 0, diff;
    if (n > 0) {
        do {
            --n;
            diff = pm1[n] - pm2[n];
            res = (res & -!diff) | diff;
        } while (n != 0);
    }
    return (res > 0) - (res < 0);
}
```

### Fastest implementation using pointers

``` C
int cst_time_memcmp_fastest2(const void *m1, const void *m2, size_t n) 
{
    const unsigned char *pm1 = (const unsigned char*)m1 + n; 
    const unsigned char *pm2 = (const unsigned char*)m2 + n; 
    int res = 0;
    if (n > 0) {
        do {
            int diff = *--pm1 - *--pm2;
            res = (res & -!diff) | diff;
        } while (pm1 != m1);
    }
    return (res > 0) - (res < 0);
}
```

### Safest implementation using subscript

``` C
int cst_time_memcmp_safest1(const void *m1, const void *m2, size_t n) 
{
    const unsigned char *pm1 = (const unsigned char*)m1; 
    const unsigned char *pm2 = (const unsigned char*)m2; 
    int res = 0, diff;
    if (n > 0) {
        do {
            --n;
            diff = pm1[n] - pm2[n];
            res = (res & (((diff - 1) & ~diff) >> 8)) | diff;
        } while (n != 0);
    }
    return ((res - 1) >> 8) + (res >> 8) + 1;
}
```

### Safest implementation using pointers

``` C
int cst_time_memcmp_safest2(const void *m1, const void *m2, size_t n) 
{
    const unsigned char *pm1 = (const unsigned char*)m1 + n; 
    const unsigned char *pm2 = (const unsigned char*)m2 + n; 
    int res = 0;
    if (n > 0) {
        do {
            int diff = *--pm1 - *--pm2;
            res = (res & (((diff - 1) & ~diff) >> 8)) | diff;
        } while (pm1 != m1);
    }
    return ((res - 1) >> 8) + (res >> 8) + 1;
}
```

### Alternate constant time memcmp function

The following constant time `memcmp` function code was found here: ftp://ftp.icm.edu.pl/pub/NetBSD/misc/apb/consttime_memcmp.c. It was proposed to NetBSD, but finally rejected by the developpers because they didn't found any use case for such a function. Test of byte array equality is the most frequent and where constant time matters. This code is thus used as reference implementation and is unrelated to NetBSD. See issue [#2](/../../issues/2) for the discussion and clarification. 

``` C
int consttime_memcmp(const void *b1, const void *b2, size_t len)
{
    const uint8_t *c1, *c2;
    uint16_t d, r, m;
    uint16_t v;

    c1 = b1;
    c2 = b2;
    r = 0;
    while (len) {
        v = ((uint16_t)(uint8_t)r)+255;
        m = v/256-1;
        d = (uint16_t)((int)*c1 - (int)*c2);
        r |= (d & m);
        ++c1;
        ++c2;
        --len;
    }
    return (int)((int32_t)(uint16_t)((uint32_t)r + 0x8000) - 0x8000);
}
```
    

## Verification

The provided code comes with validation tests and a processing time
measurement.

We compare our code execution time with the reference function `consttime_memcmp()`. 
Unfortunately this function doesn't return -1 or 1 and thus reveal
something of the compared memory zone.

The test consist in comparing two buffers containing 1 MiB of same bytes.

- In test 1, both buffers are filled with 0.
- In test 2, 1 buffer is half filled with 0xFF and the other half with 0. The other buffer is filled with 0.
- In test 3, 1 buffer is fully filled with 0xFF, while the other buffer is filled with 0.

Each measurement performs the buffer comparision 100 times.
100 measurments are performed to compute the mean and standard deviation.


### Discussion 

The fastest code is indeed the fastest. The safest code is slightly slower but 
still faster than the `consttime_memcmp()` function. Using pointers instead
of subscript operators is also slightly faster. 

It is valid but also an overkill to use the `cst_time_memcmp` function to simply test byte array equality. A simpler and faster algoritm exist for that and you'll find this function in all good crypto library. 

While some people may not see a use case for a constant time byte array lexicographical comparison, it doesn't prove no use case exist. Nevertheless, if there are no obvious and frequent use case, it is perfectly justified to not include it in a library. Whoever need it can copy the code from here. 

### Output of the program 

    $ gcc -O3 main.c -lm && ./a.out
    Start testing
    
    ... all tests successfull ...
    
    ---- cst_time_memcmp_fastest1
    test 1 : mean=104.149000 ms stddev=0.095697  n=100
    test 2 : mean=104.121130 ms stddev=0.052406  n=100
    test 3 : mean=104.573070 ms stddev=2.732596  n=100
    x : 0xFFE95738
    ---- cst_time_memcmp_fastest2
    test 1 : mean=96.809990 ms stddev=0.037059  n=100
    test 2 : mean=97.133720 ms stddev=1.474790  n=100
    test 3 : mean=96.839680 ms stddev=0.999072  n=100
    x : 0xFFE95738
    ---- cst_time_memcmp_safest1
    test 1 : mean=116.339240 ms stddev=0.049447  n=100
    test 2 : mean=116.334330 ms stddev=0.035672  n=100
    test 3 : mean=116.335910 ms stddev=0.037543  n=100
    x : 0xFFE95738
    ---- cst_time_memcmp_safest2
    test 1 : mean=110.067570 ms stddev=0.033885  n=100
    test 2 : mean=110.075140 ms stddev=0.061724  n=100
    test 3 : mean=110.066070 ms stddev=0.030296  n=100
    x : 0xFFE95738
    ---- consttime_memcmp
    test 1 : mean=159.558610 ms stddev=0.030275  n=100
    test 2 : mean=159.561470 ms stddev=0.036409  n=100
    test 3 : mean=159.763520 ms stddev=0.358880  n=100
    x : 0xE96DE0C8
    done

    
    
