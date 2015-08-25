#include <stdio.h>


/* The cst_time_memcmp() function compares the first n bytes (each interpreted
 * as unsigned char) of the n bytes long memory areas m1 and m2 in a time
 * exactly proportional to n.
 *
 * The cst_time_memcmp() function returns an integer less than, equal to, or
 * greater than zero if the first n bytes of m1 is found, respectively,
 * to be less than, to match, or be greater than the first n bytes of
 * m2.
 *
 * For a nonzero return value, the sign is determined by the sign of the
 * difference between the first pair of bytes (interpreted as unsigned
 * char) that differ in m1 and m2.
 *
 * The return value is zero when n is zero, m1 and m2 are the same
 * memory area, a is NULL or b is NULL.
 */
int cst_time_memcmp(const void *m1, const void *m2, size_t n) {
    int res = 0;
    if (m1 != m2 && n && m1 && m2) {
        const unsigned char *pm1 = (const unsigned char*)m1 + n;
        const unsigned char *pm2 = (const unsigned char*)m2 + n;
        do {
            int c = *--pm1 - *--pm2;
            res = (((c != 0) - 1) & res) | c;
        } while (pm1 != m1);
    }
    return res;
}


const char *data_to_hex( const unsigned char *a, size_t n) {
    static char buf[100];
    if (!a) {
        sprintf(buf, "NULL");
    } else if (n < 33) {
        char *p = buf;
        for (size_t i = 0; i < n; ++i) {
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


void test_one_cst_time_memcmp(const unsigned char *a, const unsigned char *b, size_t n, int test) {
    puts("");
    printf("a(%p)= %s\n", a, data_to_hex(a, 5));
    printf("b(%p)= %s\n", b, data_to_hex(b, 5));

    if (test < 0) {
        if (cst_time_memcmp(a, b, n) < 0) {
            printf("SUCCESS test a < b\n");
        } else {
            printf("FAIL ! test a < b\n");
        }
    } else if (test == 0) {
        if (cst_time_memcmp(a, b, n)==0) {
            printf("SUCCESS test a == b\n");
        } else {
            printf("FAIL ! test a == b\n");
        }
    } else {
        if (cst_time_memcmp(a, b, n) > 0) {
            printf("SUCCESS test a > b\n");
        } else {
            printf("FAIL ! test a > b\n");
        }
    }
}


const unsigned char d[5][5] = {
    { 0x12, 0x34, 0x56, 0x78, 0x90 },
    { 0x12, 0x34, 0x56, 0x78, 0x90 },
    { 0x12, 0x34, 0x56, 0x90, 0x78 },
    { 0x12, 0x90, 0x34, 0x56, 0x78 },
    { 0x90, 0x12, 0x34, 0x56, 0x78 }
};


void test_all_cst_time_memcmp() {
    for (size_t i = 0; i < 5; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            if (i != j) {
                int test = (i < 2 && j < 2) ? 0 : i - j;
                test_one_cst_time_memcmp(d[i], d[j], 5, test);
            }
        }
    }
}


int main(void)
{
    puts("Start testing");
    test_all_cst_time_memcmp();
    puts("done");
    return 0;
}

