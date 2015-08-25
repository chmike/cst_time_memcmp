#include <stdio.h>
#include <sys/time.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>

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
int cst_time_memcmp(const void *m1, const void *m2, size_t n)
{
    int res = 0;
    if (m1 != m2 && n && m1 && m2) {
        const unsigned char *pm1 = (const unsigned char *)m1 + n;
        const unsigned char *pm2 = (const unsigned char *)m2 + n;
        do {
            int c = *--pm1 - *--pm2;
            // if (c != 0) res = c;
            res = (-!c & res) | c;
        } while (pm1 != m1);
    }
    return ((res >= 0) + (res > 0)) - 1;
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


void test_one_cst_time_memcmp(const unsigned char *a, const unsigned char *b,
                              size_t n, int test)
{
    puts("");
    printf("        a(%p)= %s\n", a, data_to_hex(a, 5));
    printf("        b(%p)= %s\n", b, data_to_hex(b, 5));
    if (test < 0) {
        if (cst_time_memcmp(a, b, n) == -1) {
            printf("SUCCESS test a < b\n");
        } else {
            printf("FAIL ! test a < b\n");
        }
    } else if (test == 0) {
        if (cst_time_memcmp(a, b, n) == 0) {
            printf("SUCCESS test a == b\n");
        } else {
            printf("FAIL ! test a == b\n");
        }
    } else {
        if (cst_time_memcmp(a, b, n) == 1) {
            printf("SUCCESS test a > b\n");
        } else {
            printf("FAIL ! test a > b\n");
        }
    }
}


const unsigned char d[6][5] = {
    { 0x12, 0x34, 0x56, 0x78, 0x90 },
    { 0x12, 0x34, 0x56, 0x78, 0x90 },
    { 0x12, 0x34, 0x56, 0x90, 0x78 },
    { 0x12, 0x34, 0x78, 0x56, 0x90 },
    { 0x12, 0x90, 0x34, 0x56, 0x78 },
    { 0x90, 0x12, 0x34, 0x56, 0x78 }
};


void test_all_cst_time_memcmp()
{
    size_t i, j, n = 6, l = 5;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            if (i != j) {
                int test = (i < 2 && j < 2) ? 0 : i - j;
                test_one_cst_time_memcmp(d[i], d[j], l, test);
            }
        }
    }
}

int consttime_memcmp(const void *m1, const void *m2, size_t n);

int test_time()
{
    struct timeval t1, t2;
    size_t n = 1024 * 1024, i, j, m = 100, nm = 10000;
    unsigned char *buf1 = malloc(n), *buf2 = malloc(n);
    size_t nval = 0;
    double mean = 0.0, m2 = 0.0;
    int x = 0; // To avoid that loops are optimized out
    memset(buf1, 0x00, n);
    memset(buf2, 0x00, n);
    // load caches, ignore this measure
    for (j = 0; j < nm; ++j) {
        double elapsed_time_ms;
        gettimeofday(&t1, NULL);
        for (i = 0; i < m; ++i) {
            x += cst_time_memcmp(buf1, buf2, n) * i;
        }
        gettimeofday(&t2, NULL);
        elapsed_time_ms = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
        elapsed_time_ms += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
        ++nval;
        double delta = elapsed_time_ms - mean;
        mean += delta / nval;
        m2 += delta * (elapsed_time_ms - mean);
    }
    m2 /= (nval - 1);
    nval = 0;
    mean = m2 = 0.0;
    for (j = 0; j < nm; ++j) {
        double elapsed_time_ms;
        gettimeofday(&t1, NULL);
        for (i = 0; i < m; ++i) {
            x += cst_time_memcmp(buf1, buf2, n) * i;
        }
        gettimeofday(&t2, NULL);
        elapsed_time_ms = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
        elapsed_time_ms += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
        ++nval;
        double delta = elapsed_time_ms - mean;
        mean += delta / nval;
        m2 += delta * (elapsed_time_ms - mean);
    }
    m2 /= (nval - 1);
    printf("Buffer 1 filled with 0x%02X and buffer 2 with 0x%02X :\n",
           buf1[0], buf2[0]);
    printf("elapsed time : mean=%f ms stddev=%f  n=%zu\n\n",
           mean, sqrt(m2), nm);
    nval = 0;
    mean = m2 = 0.0;
    memset(buf1, 0x00, n);
    memset(buf2, 0xFF, n);
    for (j = 0; j < nm; ++j) {
        double elapsed_time_ms;
        gettimeofday(&t1, NULL);
        for (i = 0; i < m; ++i) {
            x += cst_time_memcmp(buf1, buf2, n) * i;
        }
        gettimeofday(&t2, NULL);
        elapsed_time_ms = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
        elapsed_time_ms += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
        ++nval;
        double delta = elapsed_time_ms - mean;
        mean += delta / nval;
        m2 += delta * (elapsed_time_ms - mean);
    }
    m2 /= (nval - 1);
    printf("Buffer 1 filled with 0x%02X and buffer 2 with 0x%02X :\n",
           buf1[0], buf2[0]);
    printf("elapsed time : mean=%f ms stddev=%f  n=%zu\n\n",
           mean, sqrt(m2), nm);

    nval = 0;
    mean = m2 = 0.0;
    memset(buf1, 0x00, n);
    memset(buf2, 0xFF, n);
    for (j = 0; j < nm; ++j) {
        double elapsed_time_ms;
        gettimeofday(&t1, NULL);
        for (i = 0; i < m; ++i) {
            x += consttime_memcmp(buf1, buf2, n) * i;
        }
        gettimeofday(&t2, NULL);
        elapsed_time_ms = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
        elapsed_time_ms += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
        ++nval;
        double delta = elapsed_time_ms - mean;
        mean += delta / nval;
        m2 += delta * (elapsed_time_ms - mean);
    }
    m2 /= (nval - 1);
    puts("consttime_memcmp() test");
    printf("Buffer 1 filled with 0x%02X and buffer 2 with 0x%02X :\n",
           buf1[0], buf2[0]);
    printf("elapsed time : mean=%f ms stddev=%f  n=%zu\n\n",
           mean, sqrt(m2), nm);

    return x;
}


int main(void)
{
    puts("Start testing");
    test_all_cst_time_memcmp();
    puts("-------------------------------------------------");
    test_time();
    puts("done");
    return 0;
}

