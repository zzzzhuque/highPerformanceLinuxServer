//
// Created by zhuque on 2021/3/9.
//
#include <cstdio>

void byteOrder() {
    union {
        short value;
        char unionBytes[sizeof(short)];
    } test;
    test.value = 0x0102;
    if ((test.unionBytes[0] == 1) && (test.unionBytes[1] == 2)) {
        printf("big endian\n");
    } else if ((test.unionBytes[0] == 2) && (test.unionBytes[1] == 1)) {
        printf("little endian\n");
    } else {
        printf("unknown...\n");
    }
}

int main(void) {
    byteOrder();
    return 0;
}