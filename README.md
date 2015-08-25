# Constant time memcmp() function


    int cst_time_memcmp(const void *m1, const void *m2, size_t n)


The `cst_time_memcmp()` function compares the first `n` bytes (each interpreted
as unsigned char) of the `n` bytes long memory areas `m1` and `m2` in a time
directly proportional to `n`.

The `cst_time_memcmp()` function returns 0 if the `n` first bytes of `m1` and 
`m2` are equal. If the first different byte is foind at index `k`, the function
returns -1 if `m1[k] < m2[k]`, and +1 if `m1[k] > m2[k]`. Appart from the 
comparision result, this function reveals nothing on `m1` or `m2`.


The function returns also 0 when at least one of the following conditions is true.

- `n` is zero ;
- `m1` and `m2` are the same memory area ;
- `m1` is NULL ;
- `m2` is NULL ;



## Algorithm

The following code illustrate the none constant time memcmp() algoritm's logic. 

    int memcmp(const unsigned char *m1, const unsigned char *m1, size_t n) {
        size_t i;
        for (i = 0; i < n; ++i ) {
            int tmp = m1[i] - m2[i];
            if (tmp != 0)
                return (tmp < 0) ? -1 : +1;
        }
        return 0;
    }

The constant time memcmp() function's algorithm is illustrated below

    int memcmp(const unsigned char *m1, const unsigned char *m1, size_t n) {
        int res = 0;
        if (n > 0) {
            do {
                --n;
                int tmp = m1[n] - m2[n];
                if (tmp != 0)
                    res = tmp;
            } while (n != 0);
        }
        if (res == 0)
            return 0;
        return (res < 0) ? -1 : +1;
    }

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

    if (tmp != 0)
        res = tmp;

is replaced by

    res = (-!tmp & res) | tmp;
    
The computation of the return value is also different.

    if (res == 0)
        return 0;
    return (res < 0) ? -1 : +1;

is replaced by

    return ((res >= 0) + (res > 0)) - 1;



## Verification

The provided code comes with validation tests and a processing time
measurement.

I also added another constant time memcmp() function (NetBSD) to 
compare performance. This function is `consttime_memcmp()` which 
has been measured to 161 times slower.


## Output of the program 

    $ gcc -O2 consttime_memcmp.c main.c -lm && ./a.out 
    Start testing
    
            a(0x401230)= 12 34 56 78 90
            b(0x401235)= 12 34 56 78 90
    SUCCESS test a == b
    
            a(0x401230)= 12 34 56 78 90
            b(0x40123a)= 12 34 56 90 78
    SUCCESS test a < b
    
            a(0x401230)= 12 34 56 78 90
            b(0x40123f)= 12 34 78 56 90
    SUCCESS test a < b
    
            a(0x401230)= 12 34 56 78 90
            b(0x401244)= 12 90 34 56 78
    SUCCESS test a < b
    
            a(0x401230)= 12 34 56 78 90
            b(0x401249)= 90 12 34 56 78
    SUCCESS test a < b
    
            a(0x401235)= 12 34 56 78 90
            b(0x401230)= 12 34 56 78 90
    SUCCESS test a == b
    
            a(0x401235)= 12 34 56 78 90
            b(0x40123a)= 12 34 56 90 78
    SUCCESS test a < b
    
            a(0x401235)= 12 34 56 78 90
            b(0x40123f)= 12 34 78 56 90
    SUCCESS test a < b
    
            a(0x401235)= 12 34 56 78 90
            b(0x401244)= 12 90 34 56 78
    SUCCESS test a < b
    
            a(0x401235)= 12 34 56 78 90
            b(0x401249)= 90 12 34 56 78
    SUCCESS test a < b
    
            a(0x40123a)= 12 34 56 90 78
            b(0x401230)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x40123a)= 12 34 56 90 78
            b(0x401235)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x40123a)= 12 34 56 90 78
            b(0x40123f)= 12 34 78 56 90
    SUCCESS test a < b
    
            a(0x40123a)= 12 34 56 90 78
            b(0x401244)= 12 90 34 56 78
    SUCCESS test a < b
    
            a(0x40123a)= 12 34 56 90 78
            b(0x401249)= 90 12 34 56 78
    SUCCESS test a < b
    
            a(0x40123f)= 12 34 78 56 90
            b(0x401230)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x40123f)= 12 34 78 56 90
            b(0x401235)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x40123f)= 12 34 78 56 90
            b(0x40123a)= 12 34 56 90 78
    SUCCESS test a > b
    
            a(0x40123f)= 12 34 78 56 90
            b(0x401244)= 12 90 34 56 78
    SUCCESS test a < b
    
            a(0x40123f)= 12 34 78 56 90
            b(0x401249)= 90 12 34 56 78
    SUCCESS test a < b
    
            a(0x401244)= 12 90 34 56 78
            b(0x401230)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x401244)= 12 90 34 56 78
            b(0x401235)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x401244)= 12 90 34 56 78
            b(0x40123a)= 12 34 56 90 78
    SUCCESS test a > b
    
            a(0x401244)= 12 90 34 56 78
            b(0x40123f)= 12 34 78 56 90
    SUCCESS test a > b
    
            a(0x401244)= 12 90 34 56 78
            b(0x401249)= 90 12 34 56 78
    SUCCESS test a < b
    
            a(0x401249)= 90 12 34 56 78
            b(0x401230)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x401249)= 90 12 34 56 78
            b(0x401235)= 12 34 56 78 90
    SUCCESS test a > b
    
            a(0x401249)= 90 12 34 56 78
            b(0x40123a)= 12 34 56 90 78
    SUCCESS test a > b
    
            a(0x401249)= 90 12 34 56 78
            b(0x40123f)= 12 34 78 56 90
    SUCCESS test a > b
    
            a(0x401249)= 90 12 34 56 78
            b(0x401244)= 12 90 34 56 78
    SUCCESS test a > b
    -------------------------------------------------
    Buffer 1 filled with 0x00 and buffer 2 with 0x00 :
    elapsed time : mean=1.010604 ms stddev=0.016005  n=10000
    
    Buffer 1 filled with 0x00 and buffer 2 with 0xFF :
    elapsed time : mean=1.012818 ms stddev=0.019014  n=10000
    
    consttime_memcmp() test
    Buffer 1 filled with 0x00 and buffer 2 with 0xFF :
    elapsed time : mean=161.510460 ms stddev=2.666180  n=10000
    
    done
