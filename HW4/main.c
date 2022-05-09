#include <stdio.h>
#include "mm.h"

int main() {
    int *a = mymalloc(sizeof(int) * 2);
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    printf("%p\n", a);
    printf("%d\n", a[0]);
    printf("%d\n", a[1]);
    printf("%d\n", a[2]);

    a = myrealloc(a, sizeof(int) * 3);
    printf("%p\n", a);
    printf("%d\n", a[0]);
    printf("%d\n", a[1]);
    printf("%d\n", a[2]);

    int *c = mycalloc(10, sizeof(int));
    printf("%p\n", &c[0]);
    printf("%p\n", &c[1]);
    printf("%p\n", &c[2]);
    printf("%p\n", &c[3]);
    printf("%p\n", &c[4]);
    printf("%p\n", &c[5]);
    printf("%p\n", &c[6]);
    printf("%p\n", &c[7]);
    printf("%p\n", &c[8]);
    printf("%p\n", &c[9]);

    return 0;
}