#include <stdio.h>
g(i, x, t, o) { return ((3 & x & (i * ((3 & i >> 16 ? "BY}6YB6%" : "Qj}6jQ6%")[t % 8] + 51) >> o)) << 4); };
void main(void)
{
    int i, n, s;
//    printf("Hi there\n");
    for (i = 0;; i++) {
        putchar(g(i, 1, n = i >> 14, 12) + g(i, s = i >> 17, n ^ i >> 13, 10) + g(i, s / 3, n + ((i >> 11) % 3), 10) + g(i, s / 5, 8 + n - ((i >> 10) % 3), 9));
        fflush(stdout);
//        if ((i%128)==0)
//            putchar('\n');
    }
}
