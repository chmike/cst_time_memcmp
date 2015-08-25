# Constant time memcmp() function


    int cst_time_memcmp(const void *m1, const void *m1, size_t n)


The `cst_time_memcmp()` function compares the first `n` bytes (each interpreted
as unsigned char) of the `n` bytes long memory areas `m1` and `m2` in a time
exactly proportional to `n`.

The `cst_time_memcmp()` function returns an integer less than, equal to, or
greater than zero if the first `n` bytes of `m1` are found, respectively,
to be less than, to match, or be greater than the first `n` bytes of
`m2`.

For a nonzero return value, the sign is determined by the sign of the
difference between the first pair of bytes (interpreted as unsigned
char) that differ in `m1` and `m2` (`return m1[k] - m2[k]`).

The return value is zero when at least one of the following conditions is true:

- when `n` is zero ;
- when `m1` and `m2` are the same memory area ;
- when `m1` is NULL ;
- when `m2` is NULL.


**Note** : the code using C99