#include <stdio.h>
#include <sys/time.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>


/* The cst_time_memcmp() function compares the first n bytes (each interpreted
 * as unsigned char) of the n bytes long memory areas m1 and m2 in a time
 * exactly proportional to n.
 *
 * The cst_time_memcmp() function returns 0 if the n first bytes of m1 and
 * m2 are equal. If the first different byte is found at index k, the function
 * returns -1 if m1[k] < m2[k], and +1 if m1[k] > m2[k]. Appart from the
 * comparision result, this function reveals nothing of m1 or m2.
 *
 * The function returns also 0 when at least one of the following conditions
 * is true.
 *
 * - n is zero ;
 * - m1 and m2 are the same memory area ;
 * - m1 is NULL ;
 * - m2 is NULL.
 */
int cst_time_memcmp_fastest2(const void *m1, const void *m2, size_t n)
{
    int res = 0;
    if (m1 != m2 && n && m1 && m2) {
        const unsigned char *pm1 = (const unsigned char *)m1 + n;
        const unsigned char *pm2 = (const unsigned char *)m2 + n;
        do {
            int diff = *--pm1 - *--pm2;
            // if (diff != 0) res = diff;
            res = (res & -!diff) | diff;
        } while (pm1 != m1);
    }
    return (res > 0) - (res < 0);
}

int cst_time_memcmp_fastest1(const void *m1, const void *m2, size_t n)
{
    int res = 0, diff;
    if (m1 != m2 && n && m1 && m2) {
        const unsigned char *pm1 = (const unsigned char *)m1;
        const unsigned char *pm2 = (const unsigned char *)m2;
        do {
            --n;
            diff = pm1[n] - pm2[n];
            // if (diff != 0) res = diff;
            res = (res & -!diff) | diff;
        } while (n != 0);
    }
    return (res > 0) - (res < 0);
}

int cst_time_memcmp_safest1(const void *m1, const void *m2, size_t n)
{
    int res = 0, diff;
    if (m1 != m2 && n && m1 && m2) {
        const unsigned char *pm1 = (const unsigned char *)m1;
        const unsigned char *pm2 = (const unsigned char *)m2;
        do {
            --n;
            diff = pm1[n] - pm2[n];
            // if (diff != 0) res = diff;
            res = (res & (((diff - 1) & ~diff) >> 8)) | diff;
        } while (n != 0);
    }
    return ((res - 1) >> 8) + (res >> 8) + 1;
}


int cst_time_memcmp_safest2(const void *m1, const void *m2, size_t n)
{
    int res = 0, diff;
    if (m1 != m2 && n && m1 && m2) {
        const unsigned char *pm1 = (const unsigned char *)m1 + n;
        const unsigned char *pm2 = (const unsigned char *)m2 + n;
        do {
            diff = *--pm1 - *--pm2;
            // if (diff != 0) res = diff;
            res = (res & (((diff - 1) & ~diff) >> 8)) | diff;
        } while (pm1 != m1);
    }
    return ((res - 1) >> 8) + (res >> 8) + 1;
}


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


const char *data_to_hex(const unsigned char *a, size_t n)
{
    static char buf[100];
    if (!a) {
        sprintf(buf, "NULL");
    } else if (n < 33) {
        char *p = buf;
        size_t i;
        for (i = 0; i < n; ++i) {
            p += sprintf(p, "%02X ", a[i]);
        }
        if (p != buf) {
            *--p = '\0';
        }
    } else {
        sprintf(buf, "data[%zu] too long", n);
    }
    return buf;
}


#define TEST_ONE_SMALLER(A,B,N,F) do {  \
    if (F(A, B, N) < 0)                 \
        printf("SUCCESS test a < b\n"); \
    else                                \
        printf("FAIL !  test a < b\n"); \
} while (0)

#define TEST_ONE_EQUAL(A,B,N,F) do {    \
    if (F(A, B, N) == 0)                \
        printf("SUCCESS test a = b\n"); \
    else                                \
        printf("FAIL !  test a = b\n"); \
} while (0)

#define TEST_ONE_BIGGER(A,B,N,F) do {   \
    if (F(A, B, N) > 0)                 \
        printf("SUCCESS test a > b\n"); \
    else                                \
        printf("FAIL !  test a > b\n"); \
} while (0)


const unsigned char d[6][5] = {
    { 0x12, 0x34, 0x56, 0x78, 0x90 },
    { 0x12, 0x34, 0x56, 0x78, 0x90 },
    { 0x12, 0x34, 0x56, 0x90, 0x78 },
    { 0x12, 0x34, 0x78, 0x56, 0x90 },
    { 0x12, 0x90, 0x34, 0x56, 0x78 },
    { 0x90, 0x12, 0x34, 0x56, 0x78 }
};

#define TEST_ALL(F) do {                                         \
    size_t i, j, n = 6, l = 5;                                   \
    printf("---- %s\n", #F );                                    \
    for (i = 0; i < n; ++i) {                                    \
        for (j = 0; j < n; ++j) {                                \
            if (i == j)                                          \
                continue;                                        \
            puts("");                                            \
            printf("        a(%p)= %s\n", d[i], data_to_hex(d[i], l)); \
            printf("        b(%p)= %s\n", d[j], data_to_hex(d[j], l)); \
            if (i < 2 && j < 2)                                  \
                TEST_ONE_EQUAL(d[i], d[j], l, F);                \
            else if (i < j)                                      \
                TEST_ONE_SMALLER(d[i], d[j], l, F);              \
            else                                                 \
                TEST_ONE_BIGGER(d[i], d[j], l, F);               \
        }                                                        \
    }                                                            \
} while (0)


#define TEST_TIME_ONE(A,B,K,L,N,F,MEAN,VAR) do{                \
    struct timeval t1, t2;                                     \
    size_t i,j, nval=0;                                        \
    double m1 = 0.0, m2 = 0.0;                                 \
    int tmp = x;                                               \
    for (j = 0; j < N; ++j) {                                  \
        double elapsed_time_ms;                                \
        gettimeofday(&t1, NULL);                               \
        for (i = 0; i < L; ++i) {                              \
            tmp += F(A, B, K) * i;                             \
        }                                                      \
        gettimeofday(&t2, NULL);                               \
        elapsed_time_ms = (t2.tv_sec - t1.tv_sec) * 1000.0;    \
        elapsed_time_ms += (t2.tv_usec - t1.tv_usec) / 1000.0; \
        ++nval;                                                \
        double delta = elapsed_time_ms - m1;                   \
        m1 += delta / nval;                                    \
        m2 += delta * (elapsed_time_ms - m1);                  \
    }                                                          \
    VAR = m2 / (nval - 1);                                     \
    MEAN = m1;                                                 \
    x += tmp;                                                  \
} while(0)


#define TEST_TIME(F) do {                                      \
    size_t k = 1024*1024, l = 100, n = 100;                    \
    unsigned char *buf1 = malloc(k), *buf2 = malloc(k);        \
    double mean = 0.0, var = 0.0;                              \
    int x = 0;                                                 \
    memset(buf1, 0, k);                                        \
    memset(buf2, 0, k);                                        \
    printf("---- %s\n", #F );                                  \
    TEST_TIME_ONE(buf1, buf2, k, l, n, F, mean, var);          \
    TEST_TIME_ONE(buf1, buf2, k, l, n, F, mean, var);          \
    printf("test 1 : mean=%f ms stddev=%f  n=%zu\n",           \
           mean, sqrt(var), n);                                \
    memset(buf2+k/2, 0xFF, k-k/2);                             \
    TEST_TIME_ONE(buf1, buf2, k, l, n, F, mean, var);          \
    printf("test 2 : mean=%f ms stddev=%f  n=%zu\n",           \
           mean, sqrt(var), n);                                \
    memset(buf2, 0xFF, k);                                     \
    TEST_TIME_ONE(buf1, buf2, k, l, n, F, mean, var);          \
    printf("test 3 : mean=%f ms stddev=%f  n=%zu\n",           \
           mean, sqrt(var), n);                                \
    printf("x : 0x%08X\n", x);                                 \
} while (0);


int main(void)
{
    puts("Start testing");

    TEST_ALL(cst_time_memcmp_fastest1);
    TEST_ALL(cst_time_memcmp_fastest2);
    TEST_ALL(cst_time_memcmp_safest1);
    TEST_ALL(cst_time_memcmp_safest2);
    TEST_ALL(consttime_memcmp);
    puts("");

    TEST_TIME(cst_time_memcmp_fastest1);
    TEST_TIME(cst_time_memcmp_fastest2);
    TEST_TIME(cst_time_memcmp_safest1);
    TEST_TIME(cst_time_memcmp_safest2);
    TEST_TIME(consttime_memcmp);

    puts("done");
    return 0;
}

